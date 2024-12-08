cmake_minimum_required(VERSION 3.16)

project(Live2DSprite)

# Set directory paths.
set(CORE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Core)
set(FRAMEWORK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Framework)
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(GLEW_PATH ${THIRD_PARTY_PATH}/glew)

# Surpress GLEW and GLFW process.
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION ${CORE_PATH}/lib/linux/x86_64/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Add GLEW.
add_subdirectory(${GLEW_PATH}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew)

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE OpenGL)
# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/framework)
# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_LINUX_GL)
# Add include path of GLEW to framework.
target_include_directories(Framework PUBLIC ${GLEW_PATH}/include)
# Link libraries to framework.
target_link_libraries(Framework Live2DCubismCore glew_s)

# Find opengl libraries.
find_package(OpenGL REQUIRED)

# Find SDL libraries
find_package(SDL2 REQUIRED)

add_library(Live2DManager STATIC)
# Add source files.
add_subdirectory(src)
# Link libraries to app.
target_link_libraries(Live2DManager
  SDL2::SDL2
  Framework
)
# Specify include directories.
target_include_directories(Live2DManager PRIVATE ${STB_PATH})

add_custom_command(
	TARGET Live2DManager
	POST_BUILD
	COMMAND ar x ${CMAKE_CURRENT_BINARY_DIR}/framework/libFramework.a
	COMMAND ar x ${CORE_PATH}/lib/linux/x86_64/libLive2DCubismCore.a
	COMMAND ar x ${CMAKE_CURRENT_BINARY_DIR}/lib/libGLEW.a
	COMMAND ar x ${CMAKE_CURRENT_BINARY_DIR}/libLive2DManager.a
	COMMAND ar r lib${PROJECT_NAME}.a *.o
	COMMAND rm -f *.o
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/include
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/Live2DManager.hpp ${CMAKE_CURRENT_BINARY_DIR}/include
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/Live2DLipSync.hpp ${CMAKE_CURRENT_BINARY_DIR}/include
	COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Samples/Resources ${CMAKE_CURRENT_BINARY_DIR}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.a DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(TEST_NAME live2d-sprite)
add_executable(${TEST_NAME})
set(TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test)
target_link_libraries(${TEST_NAME}
	Live2DManager
)
add_subdirectory(test)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)

