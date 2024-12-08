#ifndef LIVE2D_MODEL
#define LIVE2D_MODEL

#include <cstddef>
#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <CubismModelSettingJson.hpp>

#include "Type/CubismBasicType.hpp"
#include "Live2DLipSync.hpp"

class Live2DModel : 
    public Csm::CubismUserModel
{
public:
    Live2DModel(std::string model_name, std::string model_path);
    ~Live2DModel();

    void LoadModelConfig(const Csm::csmChar* config_filename);
    void SetupModel();
    void ReleaseModel();
    void SetLipSync(Live2DLipSync *handle);
    void Update(SDL_Window *window);


private:
    struct TextureInfo
    {
        GLuint id;
        int width;
        int height;
        std::string file_name;
    };
    
    Csm::csmVector<TextureInfo*> textures;

    std::string model_name;
    std::string model_path;

    Csm::csmFloat32 delay_seconds;
    Csm::CubismModelSettingJson* model_json;
    Csm::csmVector<Csm::CubismIdHandle> eyeblink_ids;
    Csm::csmVector<Csm::CubismIdHandle> lipsync_ids;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> motions;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*> expressions;
    
    const Csm::csmChar* motion_group_idle = "Idle";
    const Csm::csmChar* motion_group_tapbody = "TapBody"; 
    const Csm::csmInt32 priority_none = 0;
    const Csm::csmInt32 priority_idle = 1;
    const Csm::csmInt32 priority_normal = 2;
    const Csm::csmInt32 priority_force = 3;

    const Csm::CubismId* id_param_angleX;
    const Csm::CubismId* id_param_angleY;
    const Csm::CubismId* id_param_angleZ;
    const Csm::CubismId* id_param_bodyangleX;
    const Csm::CubismId* id_param_eyeballX;
    const Csm::CubismId* id_param_eyeballY;

    Live2DLipSync *lipsync;
    
    TextureInfo* CreateTextureFromPngFile(std::string file_name);
    void ReleaseTextures();
    void SetupTextures();

    void PreloadMotionGroup(const Csm::csmChar* group);
    Csm::CubismMotionQueueEntryHandle StartMotion(
        const Csm::csmChar* group,
        Csm::csmInt32 no,
        Csm::csmInt32 priority,
        Csm::ACubismMotion::FinishedMotionCallback onfinished_motion_handler = NULL
    );
    void ReleaseMotions();
    void SetExpression(const Csm::csmChar *expression_id);
    void ReleaseExpressions();
    void ModelParamUpdate();
}; // Live2DModel

#endif
