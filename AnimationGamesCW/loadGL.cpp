//
//  loadGL.cpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 21/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "loadGL.hpp"

bool GLFW_Initialisation_function(GLFWwindow** window, int width, int height){
    if(!glfwInit()){
        std::cerr<<"GLFW failed to initialise\n";
    };
    
    //return a pointer to a type of window (which is some struct I believe)
    
    
    glfwWindowHint(GLFW_SAMPLES, 4); //Anti-Aliasing x4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//opengl version 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    
    *window = glfwCreateWindow(width, height, "Mesh deformation", NULL, NULL);
    if( *window == NULL ){//C function requires comparison with NULL not nullptr
        std::cerr<<"Window failed to create, program will now gracefully exit\n";
        glfwTerminate();
        return 0;
    }
    
    glewExperimental = true; // Needed for core profile
    glfwMakeContextCurrent(*window);
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr<<"GLEW failed to initialised\n";
        glfwTerminate();
        return 0;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(*window, GLFW_STICKY_KEYS, GL_TRUE);
    return 1;
}
