CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
project(CubeRubik VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

FIND_PACKAGE(OpenGL REQUIRED)

ADD_EXECUTABLE(CubeRubik 
    src/main.cpp
    src/glad.h
    src/glad.c
    src/khrplatform.h
    src/stb_image.h
)

add_subdirectory(
    /home/angel/Documentos/UCSP/Semestre7/ComputacionGrafica/ArchivosOpenGL/glfw-3.3.8 
    /home/angel/Documentos/UCSP/Semestre7/ComputacionGrafica/ArchivosOpenGL/glfw-3.3.8/src
)

include_directories(
    /home/angel/Documentos/UCSP/Semestre7/ComputacionGrafica/ArchivosOpenGL/glfw-3.3.8
)

TARGET_LINK_LIBRARIES(CubeRubik
    glfw
    OpenGL::GL
)