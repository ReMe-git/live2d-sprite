# Live2DNativeLibrary

live2d library build with CubismNativeSDK and OpenGL.

## Building:
1. download CubismNativeSDK and unzip it to cubism/;
2. use srcipts in thrid_party/scripts to download OpenGL libraries;
3. use "make release" to build, you will find liblive2DSprite.a in build/;
4. use "make debug" to build with debug flag;
5. run live2d in build/ to test.

## Use in another project
add these into your CMakeLists.txt
```
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Live2DSprite)

target_link_libraries(<project_name> PRIVATE
	Live2DSprite
)
target_include_directories(<project_name> PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/Live2DSprite/include
)
```
