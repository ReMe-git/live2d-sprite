# Live2DNativeLibrary

live2d library build with CubismNativeSDK and SDL.

## Require
SDL, OpenGL, CubismNativeSDK

## Building:
1. make sure you install SDL library;
1. download CubismNativeSDK and unzip it to cubism/;
2. use srcipts in thrid_party/scripts to download glew;
3. use following build commands:
```
	cmake -S ${PWD} -B ${PWD}/build -DCMAKE_BUILD_TYPE=<Release/Debug> -DCMAKE_INSTALL_PREFIX=<install-dir>
	cd build && make && make install
```
5. run live2d-sprite in build/ to test.

## Use in another project
add these into your CMakeLists.txt
```
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_library(live2DSprite STATIC IMPORTED)
set_target_properties(live2DSprite
  PROPERTIES
  IMPORTED_LOCATION <install-dir>/lib/liblive2DSprite.a
  INTERFACE_INCLUDE_DIRECTORIES <install-dir>/include
)

target_link_libraries(<app-name> PRIVATE
    live2DSprite
    SDL2::SDL2
    OpenGL::GL
)
```
