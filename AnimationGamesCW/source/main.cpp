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
#include <cmath>

#include "shader.hpp"
#include "objloader.hpp"
#include "loadGL.hpp"
#include "cameracontroller.hpp"
#include "interpolate.hpp"

int main(int argc, const char * argv[]) {

    auto width = 1280;
    auto height = 720;
    int glmajor = 4;
    int glminor = 1;
    if(argc >1){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        glmajor = atoi(argv[3]);
        glminor = atoi(argv[4]);
    }
    
    GLFWwindow* window;
    if(!GLFW_Initialisation_function(&window, width, height, glmajor, glminor)){
        return EXIT_FAILURE;
    }
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); //makes the screen white.
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    //std::string ShaderPath = "GLSL Shaders/";
    std::string ShaderPath = "/Users/Owen/Documents/Code/C++/Flexo/AnimationGamesCW/GLSL Shaders/";
    auto vertexPath = ShaderPath + "SimpleVertexShader.glsl";
    auto fragmentPath = ShaderPath + "SimpleFragmentShader.glsl";
    GLuint shapeprogramID = LoadShaders( vertexPath.c_str(), fragmentPath.c_str());
    if(shapeprogramID == 0){
        return EXIT_FAILURE;
    }
    GLuint ShapeMatID = glGetUniformLocation(shapeprogramID,"MVP");

    //std::vector<glm::vec3> vertices_a,vertices_b;
    std::vector<unsigned int> vertexIndices, unusedVI;
    std::vector<std::vector<glm::vec3> > vertices = std::vector<std::vector<glm::vec3> >(2);
    std::string Meshobj = "/Users/Owen/Documents/Code/C++/AnimationGamesCW/AnimationGamesCW/bender";
    std::string StrEnd = std::to_string(0) + ".obj";
    bool LoadResult;
    std::cout<<"Please enter the filename of the first mesh to open:\n";
    getline(std::cin,Meshobj);
    while(1){
        LoadResult = loadSimpleOBJ(Meshobj.c_str(), vertices[0], vertexIndices);
        if(LoadResult){
            break;
        }
        getline(std::cin,Meshobj);
        std::cout<<"\n";
    }
    std::cout<<"Please enter the filename of the second mesh to open:\n";
    Meshobj = "/Users/Owen/Dropbox/bender_simple2.obj";
    getline(std::cin,Meshobj);
    while(1){
        LoadResult = loadSimpleOBJ(Meshobj.c_str(), vertices[1], unusedVI);
        if(LoadResult){
            break;
        }
        getline(std::cin,Meshobj);
        std::cout<<"\n";
    }
    

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //vertices
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices[0].size()*sizeof(glm::vec3), &vertices[0][0], GL_STATIC_DRAW);
    
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertexIndices.size()*sizeof(unsigned int), &vertexIndices[0],GL_STATIC_DRAW);
    
    
    glm::mat4 ProjectionMat = glm::mat4(1.0);
    glm::mat4 ModelMat = glm::mat4(1.0);
    glm::mat4 ViewMat = glm::lookAt(
                                 glm::vec3(0,0,4), // Camera is at (0,0,-3), in World Space
                                 glm::vec3(0,0,0), // and looks at the origin
                                 glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                 );
    
    computeScalingMatrix(width,height,&ProjectionMat);
    
    glm::mat4 shapeMVP = ProjectionMat*ViewMat*ModelMat;
    ModelMat = glm::mat4(0.2f);
    ModelMat[3][0] = -2;
    ModelMat[3][1] = -2;
    ModelMat[3][3] = 1;
    
    MeshInterpolate meshinterp(vertices[0],vertices[1],vertexIndices);
    
    //A_inv and P computed here.
    meshinterp.ComputeInitialMatrices();
    
    float t = 0.0f;
    int sign = 1;
    do{
        
        std::vector<glm::vec3> newVerts = meshinterp.Interpolate(t);
        
        t+= sign*1.0f/60.0f;
        if(t>1.0f){
            t = 0.0f;
            sign *=-1;
        }
        glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0, newVerts.size()*sizeof(glm::vec3), &newVerts[0]);

        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );
        
        // Use our shader
        glUseProgram(shapeprogramID);
        glUniformMatrix4fv(ShapeMatID, 1, GL_FALSE, &shapeMVP[0][0]);

        
        // draw the triangles in wireframe mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //glVertexAttribPointer(attribute,sizeof each vertex,type,normalised?,stride,buffer offset);
//      attribute. No particular reason for 0, but must match the layout in the shader.
//      stride = distance from one entity to the distance to the next entity.

        // Draw the mesh
        glDrawElements(GL_TRIANGLES,vertexIndices.size(), GL_UNSIGNED_INT,0); // 3 indices starting at 0 -> 1 triangle
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
    glDeleteProgram(shapeprogramID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    

    return EXIT_SUCCESS;
}
