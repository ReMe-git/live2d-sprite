#pragma once
#include <SDL2/SDL.h>
#include <string>

class Live2DManager {
public:
	~Live2DManager();
	
	static Live2DManager* GetInstance();
	bool Init(SDL_Window *window);
	void Update();
	void Destroy();
	void SetModelDirectory(std::string model_directoryname);
	void LoadModel(const std::string model_name);
	void ReleaseModel();

private:
	static Live2DManager* instance;
	Live2DManager();
}; // Live2DManager
