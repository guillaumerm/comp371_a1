cmake_minimum_required(VERSION 3.12.4)
project(COMP371_Assignment_1)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BINARY_DIR build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)

add_executable(COMP371_Assignment_1 ./src/main.cpp)

FILE(COPY shaders DESTINATION "${CMAKE_BINARY_DIR}")
FILE(COPY objects DESTINATION "${CMAKE_BINARY_DIR}")

target_link_libraries(${PROJECT_NAME} glfw glm glew ${OPENGL_LIBRARIES})

