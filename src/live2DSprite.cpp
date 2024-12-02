// include(s)
#include <unistd.h>
#include <libgen.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "live2DModel.hpp"
#include "live2DUtils.hpp"
#include "live2DSprite.hpp"

// namespace(s)
// define(s)
// type(s)
// varible(s)
live2DModel *userModel;
live2DUtils live2DTools;
Csm::CubismFramework::Option cubismOption;

std::string executeAbsolutePath;
std::string modelDirectory;

SDL_Window* sdl_window;
int windowWidth, windowHeight;

// function(s)
live2DSprite::live2DSprite() {

}

live2DSprite::~live2DSprite() {

}

void live2DSprite::InitializeCubism()
{
    //setup cubism
    cubismOption.LogFunction = live2DUtils::printMessage;
    cubismOption.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
    Csm::CubismFramework::StartUp(&live2DTools, &cubismOption);

    //Initialize cubism
    Csm::CubismFramework::Initialize();
}

void live2DSprite::SetExecuteAbsolutePath()
{
    const int maximumPathBufferSize = 1024;
    char path[maximumPathBufferSize];
    ssize_t len = readlink("/proc/self/exe", path, maximumPathBufferSize - 1);

    if (len != -1)
    {
        path[len] = '\0';
    }

    executeAbsolutePath = dirname(path);
    executeAbsolutePath += "/";
}

bool live2DSprite::InitializeSystem(SDL_Window *window)
{
    if (window == NULL) {
        live2DUtils::printLogLn("Invalid SDL Window");
        return SDL_FALSE;
    }
    sdl_window = window;
    live2DUtils::printLogLn("START");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        live2DUtils::printLogLn("Can't initilize glew.");

        return GL_FALSE;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    SDL_GetWindowSize(sdl_window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    InitializeCubism();

    SetExecuteAbsolutePath();

    return GL_TRUE;
}

void live2DSprite::Release()
{
    userModel->DeleteRenderer();

    delete userModel;

    Csm::CubismFramework::Dispose();
}

void live2DSprite::LoadModel(const std::string modelName)
{
    std::string currentModelDirectory = executeAbsolutePath + modelDirectory + modelName + "/";

    userModel = new live2DModel(modelName, currentModelDirectory);

    std::string json = ".model3.json";
    std::string fileName = modelName + json;
    userModel->loadModelConfig(fileName.c_str());

}

void live2DSprite::Update() {
    int width, height;

    glViewport(0, 0, windowWidth, windowHeight);

    SDL_GetWindowSize(sdl_window, &width, &height);
    if ((windowWidth != width || windowHeight != height) && width > 0 && height > 0)
    {
        windowWidth = width;
        windowHeight = height;
        
        glViewport(0, 0, width, height);
    }

    live2DUtils::updateTime();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    userModel->update(sdl_window);

    SDL_GL_SwapWindow(sdl_window);
}

void live2DSprite::SetModelDirectory(const std::string modelDirectoryName) {
	modelDirectory = modelDirectoryName;
}
