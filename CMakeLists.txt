
cmake_minimum_required(VERSION 2.6)
project(FODPBR)

include_directories(src contrib)

add_library(fodPBR
src/fodPBR/metaHeader.h
src/fodPBR/Camera.cpp
src/fodPBR/Camera.h
src/fodPBR/Texture.cpp
src/fodPBR/Texture.h
src/fodPBR/RenderTexture.cpp
src/fodPBR/RenderTexture.h
src/fodPBR/Shader.cpp
src/fodPBR/Shader.h
src/fodPBR/Shapes.cpp
src/fodPBR/Shapes.h
)
target_link_libraries(fodPBR SDL2 opengl32 glew32)

add_executable(PBRDemo
 src/PBRDemo/main.cpp
)

target_link_libraries(PBRDemo fodPBR)