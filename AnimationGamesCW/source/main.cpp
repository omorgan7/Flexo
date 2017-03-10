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
//#include <omp.h>
#include <string>
#include <cmath>


#include "shader.hpp"
#include "objloader.hpp"
#include "loadGL.hpp"
#include "cameracontroller.hpp"
#include "meshdeform.hpp"

int global_mouse_press = -1;
bool global_left_toggle = 0;
bool global_right_toggle = 0;
bool global_mouse_toggle = 0;
double global_xpos = 0;
double global_ypos= 0;



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS){
        if(button == GLFW_MOUSE_BUTTON_LEFT){
            global_mouse_press++;
            global_left_toggle = !global_left_toggle;
            glfwGetCursorPos(window, &global_xpos, &global_ypos);
        }
        else if(button == GLFW_MOUSE_BUTTON_RIGHT){
            global_right_toggle = !global_right_toggle;
            glfwGetCursorPos(window, &global_xpos, &global_ypos);
        } 
    }
}


inline float two_norm( float x1, float y1, float x2, float y2){
    return (float) sqrt((x1 - x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

int findVertexModelIndex(std::vector<glm::vec3> vertices, float targetX, float targetY){
    auto index = 0;
    auto distance = two_norm(vertices[0].x, vertices[0].y, targetX, targetY);
    for(auto i =1; i<vertices.size(); i++){
        auto new_distance = two_norm(vertices[i].x, vertices[i].y, targetX, targetY);
        if(new_distance < distance){
            distance = new_distance;
            index = i;
        }
    }
    return index;
}



void convertScreenPointsToWorldPoints(int width, int height, glm::mat4 * inverseProjection, float * x, float * y){
    glm::vec4 translation = glm::vec4( 2.0 * global_xpos / width - 1,
                                      -2.0 * global_ypos / height + 1,0,1);
    glm::vec4 mappedTranslation = *inverseProjection*translation;
    
    *x = mappedTranslation.x*mappedTranslation.z/mappedTranslation.w;
    *y = mappedTranslation.y*mappedTranslation.z/mappedTranslation.w;

}

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
    
    Handles handle;
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); //makes the screen white.
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    std::string ShaderPath = "GLSL Shaders/";
    auto vertexPath = ShaderPath + "SimpleVertexShader.glsl";
    auto fragmentPath = ShaderPath + "SimpleFragmentShader.glsl";
    GLuint shapeprogramID = LoadShaders( vertexPath.c_str(), fragmentPath.c_str());
    if(shapeprogramID == 0){
        return EXIT_FAILURE;
    }
    
    GLuint ShapeMatID = glGetUniformLocation(shapeprogramID,"MVP");
    vertexPath = ShaderPath + "HandleVertexShader.glsl";
    fragmentPath = ShaderPath + "HandleFragmentShader.glsl";
    GLuint handleprogramID = LoadShaders( vertexPath.c_str(), fragmentPath.c_str());
    
    if(handleprogramID == 0){
        return EXIT_FAILURE;
    }
    
    GLuint handleMatID = glGetUniformLocation(handleprogramID,"MVP");
    GLuint handleColorID = glGetUniformLocation(handleprogramID,"inColor");
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> vertexIndices;

    std::string Meshobj;
    bool LoadResult;
    std::cout<<"Please enter the filename of the mesh to open:\n";
    getline(std::cin,Meshobj);
    while(1){
        LoadResult = loadSimpleOBJ(Meshobj.c_str(), vertices, vertexIndices);
        if(LoadResult){
            break;
        }
        getline(std::cin,Meshobj);
        std::cout<<"\n";
    }
    
    float temp;
    for(auto i = vertices.begin(); i !=vertices.end(); i++){
        temp = i->x;
        i->x = i->y;
        i->y = -temp;
    }
    static GLfloat cube_data[] = {
        -0.3f, 0.3f, 0.0f,
        -0.3f,-0.3f, 0.0f,
         0.3f,-0.3f, 0.0f,
         0.3f, 0.3f, 0.0f,
        -0.3f, 0.3f, 0.0f,
         0.3f,-0.3f, 0.0f
    };
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //vertices
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertexIndices.size()*sizeof(unsigned int), &vertexIndices[0],GL_STATIC_DRAW);
    
    GLuint cubebuffer;
    glGenBuffers(1,&cubebuffer);
    glBindBuffer(GL_ARRAY_BUFFER,cubebuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cube_data),cube_data,GL_STATIC_DRAW);
    
    
    glm::mat4 ProjectionMat = glm::mat4(1.0);
    glm::mat4 ModelMat = glm::mat4(1.0);
    glm::mat4 ViewMat = glm::lookAt(
                                 glm::vec3(0,0,4), // Camera is at (0,0,-3), in World Space
                                 glm::vec3(0,0,0), // and looks at the origin
                                 glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                 );
    
    computeScalingMatrix(width,height,&ProjectionMat);
    
    glm::mat4 shapeMVP = ProjectionMat*ViewMat*ModelMat;
    ModelMat = glm::mat4(0.2);
    ModelMat[3][0] = -2;
    ModelMat[3][1] = -2;
    ModelMat[3][3] = 1;
    glm::mat4 handleMVP = ProjectionMat*ViewMat*ModelMat;
    glm::mat4 inverseProjection = glm::inverse(shapeMVP);
    std::vector<glm::vec2> screenCoords = std::vector<glm::vec2>(4);
    glm::vec3 handleColor = glm::vec3(0.4f,0.8f,1.0f);
    std::vector<std::vector<size_t> > edgeNBH;
    int retval = getEdgeNeighbourHoods(&vertexIndices,&edgeNBH);
    if(retval == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    do{
        
        if(global_left_toggle){
            global_left_toggle = !global_left_toggle;
            convertScreenPointsToWorldPoints(width, height, &inverseProjection, &screenCoords[global_mouse_press%3][0], &screenCoords[global_mouse_press%3][1]);
            handle.handleIndex[global_mouse_press%3] = findVertexModelIndex(vertices, screenCoords[global_mouse_press%3][0], screenCoords[global_mouse_press%3][1]);
        }
        if(global_right_toggle){
            global_right_toggle = !global_right_toggle;
            if(global_mouse_press > 1){
                convertScreenPointsToWorldPoints(width, height, &inverseProjection, &screenCoords[3][0], &screenCoords[3][1]);
                handle.newCoords[0] = screenCoords[3][0];
                handle.newCoords[1] = screenCoords[3][1];
                handle.newHandleIndex = 0;
                auto distance = two_norm(vertices[handle.handleIndex[0]].x, vertices[handle.handleIndex[0]].y, handle.newCoords[0], handle.newCoords[1]);
                for(int i = 1; i<3; i++){
                    auto new_distance = two_norm(vertices[handle.handleIndex[i]].x, vertices[handle.handleIndex[i]].y, handle.newCoords[0], handle.newCoords[1]);
                    if(new_distance < distance){
                        distance = new_distance;
                        handle.newHandleIndex = i;
                    }
                }
                deformMesh(&vertices, &vertexIndices, &handle,&edgeNBH);
                glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
                glBufferSubData(GL_ARRAY_BUFFER,0, vertices.size()*sizeof(glm::vec3), &vertices[0]);
                screenCoords[handle.newHandleIndex][0] = handle.newCoords[0];
                screenCoords[handle.newHandleIndex][1] = handle.newCoords[1];
            }
        }
        
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );
        
        // Use our shader
        glUseProgram(shapeprogramID);
        glUniformMatrix4fv(ShapeMatID, 1, GL_FALSE, &shapeMVP[0][0]);

        
        // 1rst attribute buffer : vertices
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        

        // Draw the mesh
        glDrawElements(GL_TRIANGLES,vertexIndices.size(), GL_UNSIGNED_INT,0); // 3 indices starting at 0 -> 1 triangle
        
        
        glUseProgram(handleprogramID);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        
        //draw the 4 handles.
        for(int i = 0; i<4; i++){
            ModelMat[3].x = screenCoords[i][0];
            ModelMat[3].y = screenCoords[i][1];
            handleMVP = ProjectionMat*ViewMat*ModelMat;
            glUniformMatrix4fv(handleMatID, 1, GL_FALSE, &handleMVP[0][0]);
            if(i==3){
                handleColor = glm::vec3(1.0f,0.0f,0.0f);
            }
            else{
                handleColor = glm::vec3(0.4f,0.8f,1.0f);
            }
            glUniform3fv(handleColorID,1, &handleColor[0]);
            glBindBuffer(GL_ARRAY_BUFFER, cubebuffer);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
            glDrawArrays(GL_TRIANGLES, 0, 2*3);
        }
//        //glVertexAttribPointer(attribute,sizeof each vertex,type,normalised?,stride,buffer offset);
//        // attribute. No particular reason for 0, but must match the layout in the shader.
//        // stride = distance from one entity to the distance to the next entity.
//        
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        
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
    glDeleteBuffers(1,&cubebuffer);
    glDeleteProgram(handleprogramID);
    glDeleteProgram(shapeprogramID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    

    return EXIT_SUCCESS;
}
