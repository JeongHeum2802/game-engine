cmake_minimum_required(VERSION 3.21)

project(OpenGLGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(OpenGLGame
    src/main.cpp
)

target_link_libraries(OpenGLGame PRIVATE
    glfw
    glad::glad
    OpenGL::GL
)
