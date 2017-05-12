# Flexo

![I'm back baby](https://raw.githubusercontent.com/omorgan7/Flexo/master/results/rigid7.png)

Shape interpolation for Computer Animation and Games 2 Coursework on the EngD programme. 

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

    clang++ source/*.cpp external/*.cpp -I external -I include -I $PATH_TO_EIGEN_INCLUDE -I $PATH_TO_GLFW3_INCLUDE -I $PATH_TO_GLEW_INCLUDE -I $PATH_TO_GLM_INCLUDE -L $PATH_TO_EIGEN_LIB -L $PATH_TO_GLFW3_LIB -L $PATH_TO_GLEW_LIB -lGL -lglfw -lGLEW -Ofast

On CL:

    cl /EHsc source\*.cpp /I include /I external external\*.cpp /openmp /O2 /link /NODEFAULTLIB:"LIBCMT" "path_to_glfw3.lib" "path_to_glew32.lib" "C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86\OpenGL32.Lib"  "user32.lib" "gdi32.lib"  "shell32.lib" "msvcrt.lib"

Additionally, to run the program will need a copy of `glew32.dll` to put in `C:/Windows` and `glfw3.dll` to be put in `C:/Windows/System32`. Running `win_install.bat` with administrator priviledges will copy these two files in. 

## Running:

Run from terminal, and ensure the executable is in the same directory as the GLSL shaders folder or the program won't run. 

Args:

    flexo x_res y_res gl_major gl_minor

The program defaults to 1280 x 720, OpenGL version 4.1. If the GL version is above what the hardware supports then it needs to be set lower or the program will fail to load.

Feed the program two mesh inputs (stdin) and it will interpolate between the two compatible meshes. If the meshes aren't compatible, weird and crazy things could happen (probably an out of bounds access error somewhere). Enjoy!
