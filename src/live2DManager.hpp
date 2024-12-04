#pragma once
#include <SDL2/SDL.h>
#include <string>

class live2DManager {
public:
	~live2DManager();
	
	static live2DManager* getInstance();
	bool init(SDL_Window *window);
	void update();
	void destroy();
	void setModelDirectory(std::string modelDirectoryName);
	void loadModel(const std::string modelName);
	void releaseModel();

private:
	static live2DManager* instance;
	live2DManager();
}; // live2DManager
