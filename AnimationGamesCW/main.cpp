//
//  main.cpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 21/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include <string>


#include "shader.hpp"
#include "objloader.hpp"
#include "loadGL.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    auto width = 1280;
    auto height = 720;
    GLFWwindow* window;
    if(!GLFW_Initialisation_function(&window, width, height)){
        return EXIT_FAILURE;
    }
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); //makes the screen blue.
    // Enable depth test
    //glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    std::string ShaderPath = "/Users/Owen/Documents/Code/C++/AnimationGamesCW/AnimationGamesCW/GLSL Shaders/";
    auto vertexPath = ShaderPath + "SimpleVertexShader.glsl";
    auto fragmentPath = ShaderPath + "SimpleFragmentShader.glsl";
    GLuint programID = LoadShaders( vertexPath.c_str(), fragmentPath.c_str());
    
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> vertexIndices;
//    std::vector<glm::vec2> uvs;
//    std::vector<glm::vec3> normals;
    
    bool res = loadSimpleOBJ("/Users/Owen/Dropbox/grid2.obj", vertices, vertexIndices);
    std::cout<<res<<"\n";

    for(auto i = vertices.begin(); i !=vertices.end(); i++){
        i->x *= 0.33f;
        i->y *= 0.33f;
        i->z *= 0.33f;

    }

    //vertices
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertexIndices.size()*sizeof(unsigned int), &vertexIndices[0],GL_STATIC_DRAW);
    
    
    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );
        
        // Use our shader
        glUseProgram(programID);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        GLint posAttrib = glGetAttribLocation(programID, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
                              0, (void*)0);
        

        // Draw the triangle !
        glDrawElements(GL_LINE_LOOP, (vertices.size()-1)*3, GL_UNSIGNED_INT,0); // 3 indices starting at 0 -> 1 triangle
        
        glDisableVertexAttribArray(0);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    

    return 0;
}
