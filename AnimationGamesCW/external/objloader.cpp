#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "objloader.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadSimpleOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<unsigned int> & out_vertex_indices){
    printf("Loading OBJ file %s...\n", path);
    
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        std::cout<<"File not found. Please enter the name of a file: \n";
        return false;
    }
    
    while( 1 ){
        
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            out_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3];
            int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1],&vertexIndex[2]);
            if(matches !=3){
                std::cout<<"File can't be read by this simple parser.\n";
                fclose(file);
                return false;
            }
            
            out_vertex_indices.push_back(vertexIndex[0]-1);
            out_vertex_indices.push_back(vertexIndex[1]-1);
            out_vertex_indices.push_back(vertexIndex[2]-1);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
        
    }
    
    fclose(file);
    return true;
}

