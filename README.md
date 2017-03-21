# Bender

![I'm back baby](https://raw.githubusercontent.com/omorgan7/Bender/master/results/bender3.png)

Surface-based mesh deformation for Computer Animation and Games 2 Coursework on the EngD programme. 

Successfully compiled on Clang and Mac OS X 10.11.6 and on the Windows 7 with the CL compiler.

## Dependencies: 

This codebase has 5 dependencies:

* OpenGL
* GLFW3
* GLEW
* Eigen
* GLM, the GL Math library.

And these libraries must be linked against in the compilation stage. GLM is a header only library so simply needs to be installed and then included. 

## Compiling:

On clang:

    clang++ source/*.cpp external/*.cpp -I external -I include -I $PATH_TO_EIGEN_INCLUDE -I $PATH_TO_GLFW3_INCLUDE -I $PATH_TO_GLEW_INCLUDE -I $PATH_TO_GLM_INCLUDE -L $PATH_TO_EIGEN_LIB -L $PATH_TO_GLFW3_LIB -L $PATH_TO_GLEW_LIB -lGL -lglfw -lGLEW -Ofast -fopenmp

OpenMP is not supported on the default (xcode) version of Clang, but can be if you install LLVM seperately on `brew install llvm`.

On CL:

Todo.

## Running:

Run from terminal, and ensure the executable is in the same directory as the GLSL shaders folder or the program won't run. 

Args:

    bender x_res y_res gl_major gl_minor

The program defaults to 1280 x 720, OpenGL version 4.1. If the GL version is above what the hardware supports then it needs to be set lower or the program will fail to load.

Left click to set a control constraint, right click to move those constraints. No reset key unfortunately!
