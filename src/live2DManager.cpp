// include(s)
#include <string>
#include <unistd.h>
#include <libgen.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "live2DModel.hpp"
#include "live2DUtils.hpp"
#include "live2DManager.hpp"

// namespace(s)
// define(s)
// type(s)
// varible(s)
live2DManager* live2DManager::instance = NULL;
live2DModel* userModel;
live2DUtils util;
Csm::CubismFramework::Option option;

std::string modelDirectory;

SDL_Window* sdl_window;
int windowWidth, windowHeight;

// function(s)
live2DManager::live2DManager()
{
    userModel = NULL;
    modelDirectory = "";
}

live2DManager::~live2DManager() {
    releaseModel();
    Csm::CubismFramework::Dispose();
}

live2DManager* live2DManager::getInstance() {
	if (instance == NULL)
		instance = new live2DManager();
	return instance;
}

bool live2DManager::initializeSystem(SDL_Window *window)
{
    if (window == NULL) {
        live2DUtils::printLogLn("Invalid SDL Window");
        return false;
    }
    sdl_window = window;
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        live2DUtils::printLogLn("Can't initilize glew.");
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_GetWindowSize(sdl_window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    //setup cubism
    option.LogFunction = live2DUtils::printMessage;
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
    Csm::CubismFramework::StartUp(&util, &option);

    //Initialize cubism
    Csm::CubismFramework::Initialize();
    
    return true;
}

void live2DManager::setModelDirectory(std::string modelDirectoryName)
{
    modelDirectory = modelDirectoryName;
}

void live2DManager::loadModel(const std::string modelName)
{
    if (userModel != NULL)
        releaseModel();

    std::string currentModelDirectory = modelDirectory + modelName + "/";

    userModel = new live2DModel(modelName, currentModelDirectory);

    std::string json = ".model3.json";
    std::string fileName = modelName + json;
    userModel->loadModelConfig(fileName.c_str());
}

void live2DManager::releaseModel()
{
    if (userModel == NULL)
        return;
    
    userModel->DeleteRenderer();
    delete userModel;
}

void live2DManager::update() {
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
}
