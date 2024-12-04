#pragma once
#include <SDL2/SDL.h>
#include <string>

class live2DManager {
public:
	~live2DManager();
	
	static live2DManager* getInstance();
	bool initializeSystem(SDL_Window *window);
	void setModelDirectory(std::string modelDirectoryName);
	void loadModel(const std::string modelName);
	void releaseModel();
	void update();

private:
	static live2DManager* instance;
	live2DManager();
}; // live2DManager
