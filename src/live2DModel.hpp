#ifndef LIVE2D_MODEL
#define LIVE2D_MODEL

#include "Type/CubismBasicType.hpp"
#include <cstddef>
#include <functional>
#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <CubismModelSettingJson.hpp>

class live2DModel : 
    public Csm::CubismUserModel
{
public:
    live2DModel(std::string modelName, std::string modelPath);
    ~live2DModel();

    void loadModelConfig(const Csm::csmChar* configFileName);
    void setupModel();
    void releaseModel();
    void update(SDL_Window *window);


private:
    struct TextureInfo
    {
        GLuint id;
        int width;
        int height;
        std::string fileName;
    };
    
    Csm::csmVector<TextureInfo*> textures;

    std::string modelName;
    std::string modelPath;

    Csm::csmFloat32 delaySeconds;
    Csm::CubismModelSettingJson* modelJson;
    Csm::csmVector<Csm::CubismIdHandle> eyeBlinkIds;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> motions;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> expressions;
    
    const Csm::csmChar* motionGroupIdle = "Idle";
    const Csm::csmChar* motionGroupTapBody = "TapBody"; 
    const Csm::csmInt32 priorityNone = 0;
    const Csm::csmInt32 priorityIdle = 1;
    const Csm::csmInt32 priorityNormal = 2;
    const Csm::csmInt32 priorityForce = 3;

    const Csm::CubismId* _idParamAngleX; // ParamAngleX
    const Csm::CubismId* _idParamAngleY; // ParamAngleX
    const Csm::CubismId* _idParamAngleZ; // ParamAngleX
    const Csm::CubismId* _idParamBodyAngleX; // ParamBodyAngleX
    const Csm::CubismId* _idParamEyeBallX; // ParamEyeBallX
    const Csm::CubismId* _idParamEyeBallY; // ParamEyeBallXY

    TextureInfo* createTextureFromPngFile(std::string fileName);
    void releaseTextures();
    void setupTextures();

    void preloadMotionGroup(const Csm::csmChar* group);
    Csm::CubismMotionQueueEntryHandle startMotion(
        const Csm::csmChar* group,
        Csm::csmInt32 no,
        Csm::csmInt32 priority,
        Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL
    );
    void modelParamUpdate();
}; // live2DModel

#endif