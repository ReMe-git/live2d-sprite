#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include "live2DSprite.hpp"


int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return 1;
	
	SDL_Window *window = SDL_CreateWindow("live2d-sprite",
		0,
		0,
		600, 800,
		SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL
	);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		printf("Failed make content, %s", SDL_GetError());
		return 1;
	}
	
	SDL_GL_MakeCurrent(window, context);

	live2DSprite sprite;
	if (sprite.InitializeSystem(window) == false)
		return 0;
	sprite.SetModelDirectory("Resources/");
	sprite.LoadModel("Hiyori");

	bool isQuit = false;
	while (!isQuit) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				isQuit = true;
		}
		sprite.Update();
	}

	sprite.Release();
}
