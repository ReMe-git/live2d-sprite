cmake_minimum_required(VERSION 3.16)

# Set lib name.
set(LIB_NAME Live2DSprite)

# Set directory paths.
set(CORE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Core)
set(FRAMEWORK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Framework)
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(GLEW_PATH ${THIRD_PARTY_PATH}/glew)
set(GLFW_PATH ${THIRD_PARTY_PATH}/glfw)

# Force static library.
add_library(${LIB_NAME} STATIC)

# Surpress GLEW and GLFW process.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
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

# Add GLEW ,GLFW.
add_subdirectory(${GLEW_PATH}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew)
add_subdirectory(${GLFW_PATH} ${CMAKE_CURRENT_BINARY_DIR}/glfw)

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

# Add source files.
add_subdirectory(src)
# Link libraries to app.
target_link_libraries(${LIB_NAME}
  Framework
  glfw
  ${OPENGL_LIBRARIES}
)
# Specify include directories.
target_include_directories(${LIB_NAME} PRIVATE ${STB_PATH})

add_custom_command(
	TARGET ${LIB_NAME}
	POST_BUILD
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/include
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/Live2DSprite.hpp ${CMAKE_CURRENT_BINARY_DIR}/include
	COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Samples/Resources ${CMAKE_CURRENT_BINARY_DIR}
)


set(TEST_NAME live2d-sprite)
add_executable(${TEST_NAME})
set(TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test)
target_link_libraries(${TEST_NAME}
	${LIB_NAME}
)
add_subdirectory(test)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
