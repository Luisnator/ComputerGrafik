cmake_minimum_required (VERSION 2.6)
project (Blatt01)

# list of source files to compile
set(sources main.cpp GLSLProgram.cpp)

# find/include libraries
find_package(OpenGL REQUIRED)
# automatically finding GLEW GLFW GLM ..
#find_package(GLEW REQUIRED)
#find_package(GLFW REQUIRED)
#find_package(GLM REQUIRED)

# use static lib for GLEW
add_definitions(-DGLEW_STATIC)
include_directories(${PROJECT_SOURCE_DIR}/libs/glew/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/glm)
# please change these for your platform!
# here: vs2017_x64/Release
link_directories (${PROJECT_SOURCE_DIR}/libs/glew/lib/vs2017_x64/Release)
link_directories (${PROJECT_SOURCE_DIR}/libs/glfw/lib/vs2017_x64/Release)

# add the executable
add_executable (Blatt01 ${sources})
target_link_libraries(Blatt01 ${OPENGL_LIBRARIES} libglew32.lib glfw3.lib)
# copy the shader/ directory relative to the executable
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
