// include(s)
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <Utils/CubismString.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include <Id/CubismIdManager.hpp>

#include "Type/CubismBasicType.hpp"
#include "Type/csmString.hpp"
#include "Type/csmVector.hpp"
#include "live2DUtils.hpp"
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#include "stb_image.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "live2DModel.hpp"

// namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace DefaultParameterId;

// define(s)
// type(s)
// varible(s)
// function(s)
csmByte* createBuffer(const csmChar* path, csmSizeInt* size) {
    return live2DUtils::loadFileAsBytes(path, size);
}

void releaseBuffer(csmByte *buffer) {
    live2DUtils::releaseBytes(buffer);
}

live2DModel::TextureInfo* live2DModel::createTextureFromPngFile(std::string fileName) {
    //search loaded texture already.
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++)
    {
        if (textures[i]->fileName == fileName)
        {
            return textures[i];
        }
    }

    GLuint textureId;
    int width, height, channels;
    unsigned int size;
    unsigned char* png;
    unsigned char* address;

    address = live2DUtils::loadFileAsBytes(fileName, &size);

    png = stbi_load_from_memory(
        address,
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);
    {

#ifdef PREMULTIPLIED_ALPHA_ENABLE
        unsigned int* fourBytes = reinterpret_cast<unsigned int*>(png);
        for (int i = 0; i < width * height; i++)
        {
            unsigned char* p = png + i * 4;
            fourBytes[i] = Premultiply(p[0], p[1], p[2], p[3]);
        }
#endif
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(png);
    live2DUtils::releaseBytes(address);

    TextureInfo* textureInfo = new live2DModel::TextureInfo();
    if (textureInfo != NULL)
    {
        textureInfo->fileName = fileName;
        textureInfo->width = width;
        textureInfo->height = height;
        textureInfo->id = textureId;

        textures.PushBack(textureInfo);
    }

    return textureInfo;
}

void live2DModel::releaseTextures() {
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++)
    {
        delete textures[i];
    }

    textures.Clear();
}

void live2DModel::setupTextures()
{
    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < modelJson->GetTextureCount(); modelTextureNumber++)
    {
        if (!strcmp(modelJson->GetTextureFileName(modelTextureNumber), ""))
        {
            continue;
        }

        csmString texturePath = modelJson->GetTextureFileName(modelTextureNumber);
        texturePath = csmString(modelPath.c_str()) + texturePath;

        TextureInfo* texture = createTextureFromPngFile(texturePath.GetRawString());
        const csmInt32 glTextueNumber = texture->id;

        GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextueNumber);
    }

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
}


live2DModel::live2DModel(const std::string modelName, const std::string modelPath)
    : CubismUserModel(),
    modelJson(NULL),
    delaySeconds(0.0f),
    modelName(modelName),
    modelPath(modelPath) {
    _idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    _idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    _idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    _idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    _idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    _idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
}

live2DModel::~live2DModel() {
    releaseModel();
}

void live2DModel::loadModelConfig(const Csm::csmChar* configFileName) {
    csmSizeInt size;
    const csmString path = csmString(modelPath.c_str()) + configFileName;

    csmByte* buffer = createBuffer(path.GetRawString(), &size);
    modelJson = new CubismModelSettingJson(buffer, size);
    releaseBuffer(buffer);

    setupModel();
}

void live2DModel::setupModel() {
    _updating = true;
    _initialized = false;

    csmByte* buffer;
    csmSizeInt size;

    if (strcmp(modelJson->GetModelFileName(), ""))
    {
        csmString path = modelJson->GetModelFileName();
        path = csmString(modelPath.c_str()) + path;

        buffer = createBuffer(path.GetRawString(), &size);
        LoadModel(buffer, size);
        releaseBuffer(buffer);
    }

    if (modelJson->GetExpressionCount() > 0)
    {
        const csmInt32 count = modelJson->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++)
        {
            csmString name = modelJson->GetExpressionName(i);
            csmString path = modelJson->GetExpressionFileName(i);
            path = csmString(modelPath.c_str()) + path;

            buffer = createBuffer(path.GetRawString(), &size);
            ACubismMotion* motion = LoadExpression(buffer, size, name.GetRawString());

            if (motion)
            {
                if (expressions[name])
                {
                    ACubismMotion::Delete(expressions[name]);
                    expressions[name] = nullptr;
                }
                expressions[name] = motion;
            }

            releaseBuffer(buffer);
        }
    }

    if (strcmp(modelJson->GetPoseFileName(), ""))
    {
        csmString path = modelJson->GetPoseFileName();
        path = csmString(modelPath.c_str()) + path;

        buffer = createBuffer(path.GetRawString(), &size);
        LoadPose(buffer, size);
        releaseBuffer(buffer);
    }

    if (strcmp(modelJson->GetPhysicsFileName(), ""))
    {
        csmString path = modelJson->GetPhysicsFileName();
        path = csmString(modelPath.c_str()) + path;

        buffer = createBuffer(path.GetRawString(), &size);
        LoadPhysics(buffer, size);
        releaseBuffer(buffer);
    }

    if (strcmp(modelJson->GetUserDataFile(), ""))
    {
        csmString path = modelJson->GetUserDataFile();
        path = csmString(modelPath.c_str()) + path;
        buffer = createBuffer(path.GetRawString(), &size);
        LoadUserData(buffer, size);
        releaseBuffer(buffer);
    }

    csmMap<csmString, csmFloat32> layout;
    modelJson->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    for (csmInt32 i = 0; i < modelJson->GetMotionGroupCount(); i++)
    {
        const csmChar* group = modelJson->GetMotionGroupName(i);
        preloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    CreateRenderer();

    setupTextures();

    _updating = false;
    _initialized = true;   
}

void live2DModel::preloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = modelJson->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = modelJson->GetMotionFileName(group, i);
        path = csmString(modelPath.c_str()) + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = createBuffer(path.GetRawString(), &size);
        CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString()));

        if (tmpMotion)
        {
            csmFloat32 fadeTime = modelJson->GetMotionFadeInTimeValue(group, i);
            if (fadeTime >= 0.0f)
            {
                tmpMotion->SetFadeInTime(fadeTime);
            }

            fadeTime = modelJson->GetMotionFadeOutTimeValue(group, i);
            if (fadeTime >= 0.0f)
            {
                tmpMotion->SetFadeOutTime(fadeTime);
            }

            if (motions[name])
            {
                ACubismMotion::Delete(motions[name]);
            }
            motions[name] = tmpMotion;
        }

        releaseBuffer(buffer);
    }
}

void live2DModel::releaseModel() {
    for (
        Csm::csmMap<Csm::csmString,
        Csm::ACubismMotion*>::const_iterator iter = motions.Begin();
        iter != motions.End(); ++iter 
    ) {
        Csm::ACubismMotion::Delete(iter->Second);
    }

    motions.Clear();

    for (
        Csm::csmMap<Csm::csmString,
        Csm::ACubismMotion*>::const_iterator iter = expressions.Begin();
        iter != expressions.End(); ++iter
    ) {
        Csm::ACubismMotion::Delete(iter->Second);
    }

    expressions.Clear();

    delete(modelJson);
}


Csm::CubismMotionQueueEntryHandle live2DModel::startMotion(
    const Csm::csmChar* group,
    Csm::csmInt32 no,
    Csm::csmInt32 priority,
    Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler
) {
     // モーション数が取得出来なかった、もしくは0の時
    if (!(modelJson->GetMotionCount(group)))
    {
        return Csm::InvalidMotionQueueEntryHandleValue;
    }

    if (priority == priorityForce)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        return Csm::InvalidMotionQueueEntryHandleValue;
    }

    const Csm::csmString fileName = modelJson->GetMotionFileName(group, no);

    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (!motion)
    {
        csmString path = fileName;
        path = csmString(modelPath.c_str()) + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = createBuffer(path.GetRawString(), &size);
        motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, onFinishedMotionHandler));

        if (motion)
        {
            csmFloat32 fadeTime = modelJson->GetMotionFadeInTimeValue(group, no);
            if (fadeTime >= 0.0f)
            {
                motion->SetFadeInTime(fadeTime);
            }

            fadeTime = modelJson->GetMotionFadeOutTimeValue(group, no);
            if (fadeTime >= 0.0f)
            {
                motion->SetFadeOutTime(fadeTime);
            }

            autoDelete = true;
        }

        releaseBuffer(buffer);
    }
    else
    {
        motion->SetFinishedMotionHandler(onFinishedMotionHandler);
    }

    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);   
}

void live2DModel::modelParamUpdate() {
    const Csm::csmFloat32 deltaTimeSeconds = live2DUtils::getDeltaTime();
    delaySeconds += deltaTimeSeconds;

    _dragManager->Update(deltaTimeSeconds);
    _dragX = _dragManager->GetX();
    _dragY = _dragManager->GetY();

    Csm::csmBool motionUpdated = false;

    _model->LoadParameters();

    if (_motionManager->IsFinished())
    {
        startMotion(motionGroupIdle, 0, priorityIdle);
    }
    else
    {
        motionUpdated = _motionManager->UpdateMotion(_model, deltaTimeSeconds);
    }

    _model->SaveParameters();

    if (_expressionManager)
    {
        _expressionManager->UpdateMotion(_model, deltaTimeSeconds);
    }

    _model->AddParameterValue(_idParamAngleX, _dragX * 30.0f);
    _model->AddParameterValue(_idParamAngleY, _dragY * 30.0f);
    _model->AddParameterValue(_idParamAngleZ, _dragX * _dragY * -30.0f);

    _model->AddParameterValue(_idParamBodyAngleX, _dragX * 10.0f);

    _model->AddParameterValue(_idParamEyeBallX, _dragX);
    _model->AddParameterValue(_idParamEyeBallY, _dragY);

    if (_physics)
    {
        _physics->Evaluate(_model, deltaTimeSeconds);
    }

    if (_pose)
    {
        _pose->UpdateParameters(_model, deltaTimeSeconds);
    }

    _model->Update();

}

void live2DModel::update(SDL_Window* window)
{
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    Csm::CubismMatrix44 projection;
    projection.LoadIdentity();

    if (_model->GetCanvasWidth() > 1.0f && width < height)
    {
        GetModelMatrix()->SetWidth(2.0f);
        projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));
    }
    else
    {
        projection.Scale(static_cast<float>(height) / static_cast<float>(width), 1.0f);
    }

    modelParamUpdate();

    if (!_model)
    {
        return;
    }

    projection.MultiplyByMatrix(_modelMatrix);

    GetRenderer<Csm::Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&projection);

    GetRenderer<Csm::Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}