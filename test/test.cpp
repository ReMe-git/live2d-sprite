#include <string>

#include "Live2DSprite.hpp"


int main(int argc, char **argv) {
	Live2DSprite app;

	if (app.InitializeSystem() == false)
		return 0;
	app.LoadModel("Hiyori");

	while (app.Update() == true);

	app.Release();
}
