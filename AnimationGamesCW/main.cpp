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
#include "cameracontroller.hpp"

int global_mouse_press = 0;
bool global_mouse_toggle = 0;
double global_xpos = 0;
double global_ypos= 0;


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        glfwGetCursorPos(window, &global_xpos, &global_ypos);
        //std::cout<<"mouse pressed at "<<global_xpos<<", "<<global_ypos<<"\n";
        global_mouse_press++;
        global_mouse_toggle = !global_mouse_toggle;
    }

}

void convertScreenPointsToWorldPoints(int width, int height, glm::mat4 * inverseProjection, float * x, float * y){
    glm::vec4 translation = glm::vec4( 2.0 * global_xpos / width - 1,
                                      -2.0 * global_ypos / height + 1,0,1);
    glm::vec4 mappedTranslation = *inverseProjection*translation;
    
    //std::cout<<ModelMat[3][0]<<" "<<ModelMat[3][1]<<"\n";
    
    *x = mappedTranslation.x*mappedTranslation.z/mappedTranslation.w;
    *y = mappedTranslation.y*mappedTranslation.z/mappedTranslation.w;

}

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
    //glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    std::string ShaderPath = "/Users/Owen/Documents/Code/C++/AnimationGamesCW/AnimationGamesCW/GLSL Shaders/";
    auto vertexPath = ShaderPath + "SimpleVertexShader.glsl";
    auto fragmentPath = ShaderPath + "SimpleFragmentShader.glsl";
    GLuint shapeprogramID = LoadShaders( vertexPath.c_str(), fragmentPath.c_str());
    GLuint ShapeMatID = glGetUniformLocation(shapeprogramID,"MVP");
    vertexPath = ShaderPath + "HandleVertexShader.glsl";
    fragmentPath = ShaderPath + "HandleFragmentShader.glsl";
    GLuint handleprogramID = LoadShaders( vertexPath.c_str(), fragmentPath.c_str());
    GLuint handleMatID = glGetUniformLocation(handleprogramID,"MVP");
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> vertexIndices;
//    std::vector<glm::vec2> uvs;
//    std::vector<glm::vec3> normals;
    
    bool res = loadSimpleOBJ("/Users/Owen/Dropbox/bender.obj", vertices, vertexIndices);
    std::cout<<res<<"\n";
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
    ModelMat = glm::mat4(0.3);
    ModelMat[3][0] = -2;
    ModelMat[3][1] = -2;
    ModelMat[3][3] = 1;
    glm::mat4 handleMVP = ProjectionMat*ViewMat*ModelMat;
    glm::mat4 inverseProjection = glm::inverse(shapeMVP);
    std::vector<glm::vec2> screenCoords = std::vector<glm::vec2>(3);
    int iterator_max = 0;
    do{
        
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
        if(global_mouse_toggle){
            global_mouse_toggle = !global_mouse_toggle;
            convertScreenPointsToWorldPoints(width, height, &inverseProjection, &screenCoords[global_mouse_press%3][0], &screenCoords[global_mouse_press%3][1]);
        }
        for(int i = 0; i<3; i++){
            ModelMat[3].x = screenCoords[i][0];
            ModelMat[3].y = screenCoords[i][1];
            handleMVP = ProjectionMat*ViewMat*ModelMat;
            glUniformMatrix4fv(handleMatID, 1, GL_FALSE, &handleMVP[0][0]);
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
    

    return 0;
}
