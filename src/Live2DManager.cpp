// include(s)
#include <string>
#include <unistd.h>
#include <libgen.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "Live2DModel.hpp"
#include "Live2DUtils.hpp"
#include "Live2DManager.hpp"

// namespace(s)
// define(s)
// type(s)
// varible(s)
Live2DManager* Live2DManager::instance = NULL;
Live2DModel* user_model;
Live2DUtils util;
Csm::CubismFramework::Option option;

std::string model_directory;

SDL_Window* sdl_window;
int window_width, window_height;

// function(s)
Live2DManager::Live2DManager()
{
    user_model = NULL;
    model_directory = "";
}

Live2DManager::~Live2DManager() {
}

Live2DManager* Live2DManager::GetInstance() {
	if (instance == NULL)
		instance = new Live2DManager();
	return instance;
}

bool Live2DManager::Init(SDL_Window *window)
{
    if (window == NULL) {
        Live2DUtils::PrintLogLn("init: Invalid SDL Window");
        return false;
    }
    sdl_window = window;
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        Live2DUtils::PrintLogLn("init: Can't initilize glew.");
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_GetWindowSize(sdl_window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    //setup cubism
    option.LogFunction = Live2DUtils::PrintMessage;
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
    Csm::CubismFramework::StartUp(&util, &option);

    //Initialize cubism
    Csm::CubismFramework::Initialize();
    
    return true;
}

void Live2DManager::Update() {
    int width, height;

    glViewport(0, 0, window_width, window_height);

    SDL_GetWindowSize(sdl_window, &width, &height);
    if ((window_width != width || window_height != height) && width > 0 && height > 0)
    {
        window_width = width;
        window_height = height;
        
        glViewport(0, 0, width, height);
    }

    Live2DUtils::UpdateTime();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    user_model->Update(sdl_window);
}

void Live2DManager::Destroy() {
    ReleaseModel();
    Csm::CubismFramework::Dispose();
}

void Live2DManager::SetModelDirectory(std::string model_directoryname)
{
    model_directory = model_directoryname;
}

void Live2DManager::LoadModel(const std::string model_name)
{
    if (user_model != NULL) {
        Live2DUtils::PrintLogLn("loadModel: There is a model running.");
        return;
	}

    std::string current_modeldirectory = model_directory + model_name + "/";

    user_model = new Live2DModel(model_name, current_modeldirectory);

    std::string json = ".model3.json";
    std::string file_name = model_name + json;
    user_model->LoadModelConfig(file_name.c_str());
}

void Live2DManager::ReleaseModel()
{
    if (user_model == NULL) {
		Live2DUtils::PrintLogLn("releaseModel: No model to release.");
        return;
	}
    
    user_model->DeleteRenderer();
    delete user_model;
}
