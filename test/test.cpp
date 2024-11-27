#include <string>

#include "Live2DSprite.hpp"


int main(int argc, char **argv) {
	Live2DSprite sprite;

	if (sprite.InitializeSystem() == false)
		return 0;
	
	sprite.SetModelDirectory("Resources/");
	sprite.LoadModel("Hiyori");

	while (true) {
		sprite.Update();
	}

	sprite.Release();
}
