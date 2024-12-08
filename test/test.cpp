#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Live2DManager.hpp"

std::string getCurrentPath(void) {
    const std::size_t MAXBUFSIZE = 2048;
    char buf[MAXBUFSIZE] = {'\0'};
    readlink("/proc/self/exe", buf, MAXBUFSIZE);
    std::string tmp = std::string(buf);    // 0 is for heap memory
	for (int i = tmp.length() - 1; tmp[i] != '/'; i--)
		tmp.pop_back();
	tmp += '/';
	return tmp;
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return 1;
	
	SDL_Window *window = SDL_CreateWindow("live2d-sprite",
		0,
		0,
		600, 800,
		SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL
	);
	if (window == NULL)
		return 1;
 
	SDL_GLContext context = SDL_GL_CreateContext(window); 
	if (context == NULL) {
		return 1;
	}
	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(1);

	std::string currentPath = getCurrentPath();
	
	if (Live2DManager::GetInstance()->Init(window) == false)
		return 0;
	
	Live2DManager::GetInstance()->SetModelDirectory(currentPath + "Resources/");
	Live2DManager::GetInstance()->LoadModel("Hiyori");
	
	bool isQuit = false;
	while (!isQuit) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				isQuit = true;
		}
		Live2DManager::GetInstance()->Update();
		SDL_GL_SwapWindow(window);
	}
	
	Live2DManager::GetInstance()->Destroy();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
}
