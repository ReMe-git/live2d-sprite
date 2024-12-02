release:
	cmake -S ${PWD} -B ${PWD}/build -DCMAKE_BUILD_TYPE=Release
	cd build && make

debug:
	cmake -S ${PWD} -B ${PWD}/build -DCMAKE_BUILD_TYPE=Debug
	cd build && make
