cmake_minimum_required(VERSION 3.21)

project(OpenGLGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(OpenGLGame
    src/main.cpp
    src/Shader.cpp
    src/Shader.h
    src/Math/Mat4.h
    src/Math/Vec3.h
    src/Math/Vec4.h
    src/Math/Transform.h
    src/Camera/Camera.h
    src/Camera/FollowCamera.h
    src/Graphics/Mesh.h
    src/Graphics/Mesh.cpp
    src/Graphics/Primitive.h
)

target_link_libraries(OpenGLGame PRIVATE
    glfw
    glad::glad
    OpenGL::GL
    glm::glm
)

set_target_properties(OpenGLGame PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:OpenGLGame>"
)

# 셰이더 복사
add_custom_command(TARGET OpenGLGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/shaders"
    "$<TARGET_FILE_DIR:OpenGLGame>/shaders"
)

# 에셋 복사
add_custom_command(TARGET OpenGLGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:OpenGLGame>/assets"
)