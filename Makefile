default:
	cmake -S ${PWD} -B ${PWD}/build -D GLFW_BUILD_WAYLAND=OFF
	cd build && make
	cp -r cubism/Samples/Resources build/
	mkdir build/include && cp src/Live2DSprite.hpp build/include
