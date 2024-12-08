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
#include <cstddef>

#include "Type/CubismBasicType.hpp"
#include "Type/csmString.hpp"
#include "Type/csmVector.hpp"
#include "Live2DUtils.hpp"
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

#include "Live2DModel.hpp"

// namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace DefaultParameterId;

// define(s)
// type(s)
// varible(s)
// function(s)
csmByte* CreateBuffer(const csmChar* path, csmSizeInt* size) {
    return Live2DUtils::LoadFileAsBytes(path, size);
}

void ReleaseBuffer(csmByte *buffer) {
    Live2DUtils::ReleaseBytes(buffer);
}

Live2DModel::TextureInfo* Live2DModel::CreateTextureFromPngFile(std::string file_name) {
    //search loaded texture already.
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++)
    {
        if (textures[i]->file_name == file_name)
        {
            return textures[i];
        }
    }

    GLuint texture_id;
    int width, height, channels;
    unsigned int size;
    unsigned char* png;
    unsigned char* address;

    address = Live2DUtils::LoadFileAsBytes(file_name, &size);

    png = stbi_load_from_memory(
        address,
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);
    {

#ifdef PREMULTIPLIED_ALPHA_ENABLE
        unsigned int* four_bytes = reinterpret_cast<unsigned int*>(png);
        for (int i = 0; i < width * height; i++)
        {
            unsigned char* p = png + i * 4;
            four_bytes[i] = Premultiply(p[0], p[1], p[2], p[3]);
        }
#endif
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(png);
    Live2DUtils::ReleaseBytes(address);

    TextureInfo* texture_info = new Live2DModel::TextureInfo();
    if (texture_info != NULL)
    {
        texture_info->file_name = file_name;
        texture_info->width = width;
        texture_info->height = height;
        texture_info->id = texture_id;

        textures.PushBack(texture_info);
    }

    return texture_info;
}

void Live2DModel::ReleaseTextures() {
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++)
    {
        delete textures[i];
    }

    textures.Clear();
}

void Live2DModel::SetupTextures()
{
    for (csmInt32 model_texture_number = 0; model_texture_number < model_json->GetTextureCount(); model_texture_number++)
    {
        if (!strcmp(model_json->GetTextureFileName(model_texture_number), ""))
        {
            continue;
        }

        csmString texturePath = model_json->GetTextureFileName(model_texture_number);
        texturePath = csmString(model_path.c_str()) + texturePath;

        TextureInfo* texture = CreateTextureFromPngFile(texturePath.GetRawString());
        const csmInt32 glTextueNumber = texture->id;

        GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(model_texture_number, glTextueNumber);
    }

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
}

void Live2DModel::SetLipSync(Live2DLipSync *handle) {
    lipsync = handle;
}

Live2DModel::Live2DModel(const std::string model_name, const std::string model_path)
    : CubismUserModel(),
    model_json(NULL),
    delay_seconds(0.0f),
    model_name(model_name),
    model_path(model_path),
    lipsync(NULL) {
    id_param_angleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    id_param_angleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    id_param_angleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    id_param_bodyangleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    id_param_eyeballX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    id_param_eyeballY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
}

Live2DModel::~Live2DModel() {
    ReleaseModel();
}

void Live2DModel::LoadModelConfig(const Csm::csmChar* config_filename) {
    csmSizeInt size;
    const csmString path = csmString(model_path.c_str()) + config_filename;

    csmByte* buffer = CreateBuffer(path.GetRawString(), &size);
    model_json = new CubismModelSettingJson(buffer, size);
    ReleaseBuffer(buffer);

    SetupModel();
}

void Live2DModel::SetupModel() {
    _updating = true;
    _initialized = false;

    csmByte* buffer;
    csmSizeInt size;

    if (strcmp(model_json->GetModelFileName(), ""))
    {
        csmString path = model_json->GetModelFileName();
        path = csmString(model_path.c_str()) + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadModel(buffer, size);
        ReleaseBuffer(buffer);
    }

    // expression
    if (model_json->GetExpressionCount() > 0)
    {
        const csmInt32 count = model_json->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++)
        {
            csmString name = model_json->GetExpressionName(i);
            csmString path = model_json->GetExpressionFileName(i);
            path = csmString(model_path.c_str()) + path;

            buffer = CreateBuffer(path.GetRawString(), &size);
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

            ReleaseBuffer(buffer);
        }
    }

    // pose
    if (strcmp(model_json->GetPoseFileName(), ""))
    {
        csmString path = model_json->GetPoseFileName();
        path = csmString(model_path.c_str()) + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadPose(buffer, size);
        ReleaseBuffer(buffer);
    }

    // physics
    if (strcmp(model_json->GetPhysicsFileName(), ""))
    {
        csmString path = model_json->GetPhysicsFileName();
        path = csmString(model_path.c_str()) + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadPhysics(buffer, size);
        ReleaseBuffer(buffer);
    }

    // userdata
    if (strcmp(model_json->GetUserDataFile(), ""))
    {
        csmString path = model_json->GetUserDataFile();
        path = csmString(model_path.c_str()) + path;
        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadUserData(buffer, size);
        ReleaseBuffer(buffer);
    }

    //eyeblink
    if (model_json->GetEyeBlinkParameterCount() > 0)
    {
        _eyeBlink = CubismEyeBlink::Create(model_json);
    }

    //breath
    {
        _breath = CubismBreath::Create();

        csmVector<CubismBreath::BreathParameterData> breath_parameters;

        breath_parameters.PushBack(CubismBreath::BreathParameterData(id_param_angleX, 0.0f, 15.0f, 6.5345f, 0.5f));
        breath_parameters.PushBack(CubismBreath::BreathParameterData(id_param_angleY, 0.0f, 8.0f, 3.5345f, 0.5f));
        breath_parameters.PushBack(CubismBreath::BreathParameterData(id_param_angleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
        breath_parameters.PushBack(CubismBreath::BreathParameterData(id_param_bodyangleX, 0.0f, 4.0f, 15.5345f, 0.5f));
        breath_parameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

        _breath->SetParameters(breath_parameters);
    }

    // eyeblinkIds
    {
        csmInt32 eyeblink_id_count = model_json->GetEyeBlinkParameterCount();
        for (csmInt32 i = 0; i < eyeblink_id_count; ++i)
        {
            eyeblink_ids.PushBack(model_json->GetEyeBlinkParameterId(i));
        }
    }

    // lipsyncIds
    {
        csmInt32 lipsync_id_count = model_json->GetLipSyncParameterCount();
        for (csmInt32 i = 0; i < lipsync_id_count; ++i)
        {
            lipsync_ids.PushBack(model_json->GetLipSyncParameterId(i));
        }
    }

    csmMap<csmString, csmFloat32> layout;
    model_json->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    for (csmInt32 i = 0; i < model_json->GetMotionGroupCount(); i++)
    {
        const csmChar* group = model_json->GetMotionGroupName(i);
        PreloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    CreateRenderer();

    SetupTextures();

    _updating = false;
    _initialized = true;   
}

void Live2DModel::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = model_json->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = model_json->GetMotionFileName(group, i);
        path = csmString(model_path.c_str()) + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        CubismMotion* tmp_motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString()));

        if (tmp_motion)
        {
            csmFloat32 fade_time = model_json->GetMotionFadeInTimeValue(group, i);
            if (fade_time >= 0.0f)
            {
                tmp_motion->SetFadeInTime(fade_time);
            }

            fade_time = model_json->GetMotionFadeOutTimeValue(group, i);
            if (fade_time >= 0.0f)
            {
                tmp_motion->SetFadeOutTime(fade_time);
            }

            if (motions[name])
            {
                ACubismMotion::Delete(motions[name]);
            }
            motions[name] = tmp_motion;
        }

        ReleaseBuffer(buffer);
    }
}

void Live2DModel::ReleaseModel() {
    ReleaseMotions();
    ReleaseExpressions();

    delete(model_json);
    delete(lipsync);
    
    ReleaseTextures();
}

Csm::CubismMotionQueueEntryHandle Live2DModel::StartMotion(
    const Csm::csmChar* group,
    Csm::csmInt32 no,
    Csm::csmInt32 priority,
    Csm::ACubismMotion::FinishedMotionCallback onfinished_motion_handler
) {
    if (!(model_json->GetMotionCount(group)))
    {
        return Csm::InvalidMotionQueueEntryHandleValue;
    }

    if (priority == priority_force)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        return Csm::InvalidMotionQueueEntryHandleValue;
    }

    const Csm::csmString file_name = model_json->GetMotionFileName(group, no);

    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (!motion)
    {
        csmString path = file_name;
        path = csmString(model_path.c_str()) + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, onfinished_motion_handler));

        if (motion)
        {
            csmFloat32 fade_time = model_json->GetMotionFadeInTimeValue(group, no);
            if (fade_time >= 0.0f)
            {
                motion->SetFadeInTime(fade_time);
            }

            fade_time = model_json->GetMotionFadeOutTimeValue(group, no);
            if (fade_time >= 0.0f)
            {
                motion->SetFadeOutTime(fade_time);
            }

            motion->SetEffectIds(eyeblink_ids, lipsync_ids);
            autoDelete = true;
        }

        ReleaseBuffer(buffer);
    }
    else
    {
        motion->SetFinishedMotionHandler(onfinished_motion_handler);
    }

    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);   
}

void Live2DModel::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = motions.Begin(); iter != motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    motions.Clear();
}

void Live2DModel::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = expressions[expressionID];
    
    if (motion != NULL)
    {
        _expressionManager->StartMotionPriority(motion, false, priority_force);
    }
}

void Live2DModel::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = expressions.Begin(); iter != expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    expressions.Clear();
}


void Live2DModel::ModelParamUpdate() {
    const Csm::csmFloat32 deltatime_seconds = Live2DUtils::GetDeltaTime();
    delay_seconds += deltatime_seconds;

    _dragManager->Update(deltatime_seconds);
    _dragX = _dragManager->GetX();
    _dragY = _dragManager->GetY();

    Csm::csmBool motion_updated = false;

    _model->LoadParameters();

    if (_motionManager->IsFinished())
    {
        StartMotion(motion_group_idle, 0, priority_idle);
    }
    else
    {
        motion_updated = _motionManager->UpdateMotion(_model, deltatime_seconds);
    }

    _model->SaveParameters();

    // opacity
    _opacity = _model->GetModelOpacity();

    // eyeblink
    if (!motion_updated)
    {
        if (_eyeBlink != NULL)
        {
            _eyeBlink->UpdateParameters(_model, deltatime_seconds);
        }
    }

    // expression
    if (_expressionManager)
    {
        _expressionManager->UpdateMotion(_model, deltatime_seconds);
    }

    _model->AddParameterValue(id_param_angleX, _dragX * 30.0f);
    _model->AddParameterValue(id_param_angleY, _dragY * 30.0f);
    _model->AddParameterValue(id_param_angleZ, _dragX * _dragY * -30.0f);

    _model->AddParameterValue(id_param_bodyangleX, _dragX * 10.0f);

    _model->AddParameterValue(id_param_eyeballX, _dragX);
    _model->AddParameterValue(id_param_eyeballY, _dragY);
    
    // breath
    if (_breath != NULL)
    {
        _breath->UpdateParameters(_model, deltatime_seconds);
    }
    // physics
    if (_physics)
    {
        _physics->Evaluate(_model, deltatime_seconds);
    }

    // lipsync
    if (_lipSync and lipsync != NULL) {
        csmFloat32 value = 0.0f;

        lipsync->Update();
        value = lipsync->GetValue();
        for (csmUint32 i = 0; i < lipsync_ids.GetSize(); i++) {
            _model->AddParameterValue(lipsync_ids[i], value, 0.8f);
        }
    }

    // pose
    if (_pose)
    {
        _pose->UpdateParameters(_model, deltatime_seconds);
    }

    _model->Update();

}

void Live2DModel::Update(SDL_Window* window)
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

    ModelParamUpdate();

    if (!_model)
    {
        return;
    }

    projection.MultiplyByMatrix(_modelMatrix);

    GetRenderer<Csm::Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&projection);

    GetRenderer<Csm::Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}
