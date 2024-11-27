default:
	cmake -S ${PWD} -B ${PWD}/build -D GLFW_BUILD_WAYLAND=OFF
	cd build && make
