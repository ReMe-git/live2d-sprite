#pragma once
#include <SDL2/SDL.h>
#include <string>

class live2DManager {
public:
	live2DManager(std::string modelDirectoryName);
	~live2DManager();
	
	bool initializeSystem(SDL_Window *window);
	void loadModel(const std::string modelName);
	void releaseModel();
	void update();

private:
}; // live2DManager
