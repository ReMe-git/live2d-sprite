#pragma once
#include <SDL2/SDL.h>
#include <string>

class live2DSprite {
private:

public:
	live2DSprite();
	~live2DSprite();
	void InitializeCubism();
	void SetExecuteAbsolutePath();
	void SetModelDirectory(const std::string modelDirectoryName);
	bool InitializeSystem(SDL_Window *window);
	void Release();
	void LoadModel(const std::string modelName);
	void Update();
}; // live2DSprite
