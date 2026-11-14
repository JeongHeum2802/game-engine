cmake_minimum_required(VERSION 3.21)

project(OpenGLGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_path(STB_INCLUDE_DIR NAMES stb_image.h REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIR NAMES miniaudio.h REQUIRED)

add_executable(OpenGLGame
    src/main.cpp
    src/Shader.cpp
    src/Shader.h
    src/Math/Mat4.h
    src/Math/Vec2.h
    src/Math/Vec3.h
    src/Math/Vec4.h
    src/Math/Transform.h
    src/Math/Ray.h
    src/Camera/Camera.h
    src/Camera/Camera.cpp
    src/Camera/FollowCamera.h
    src/Graphics/Vertex.h
    src/Graphics/Mesh.h
    src/Graphics/Mesh.cpp
    src/Graphics/Primitive.h
    src/Graphics/Model.h
    src/Graphics/Model.cpp
    src/Graphics/Texture2D.h
    src/Graphics/Texture2D.cpp
    src/Graphics/Animation/AnimatedModel.cpp
    src/Graphics/Animation/Animator.cpp
    src/Graphics/Animation/AnimatedMesh.cpp
    src/Scene/Light.h
    src/Game/Game.h
    src/Game/Game.cpp
    src/Game/Player.cpp
    src/Game/Zombie.cpp
    src/Game/ZombieSpawner.cpp 
    src/Audio/AudioManager.cpp
)

target_link_libraries(OpenGLGame PRIVATE
    glfw
    glad::glad
    OpenGL::GL
    glm::glm
    assimp::assimp
)

target_include_directories(OpenGLGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${STB_INCLUDE_DIR}
    ${MINIAUDIO_INCLUDE_DIR}
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

if(MSVC)
    target_compile_options(OpenGLGame PRIVATE /utf-8)
endif()