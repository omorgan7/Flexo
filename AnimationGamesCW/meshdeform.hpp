//
//  meshdeform.hpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 08/03/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#ifndef meshdeform_hpp
#define meshdeform_hpp

#include <Eigen/Dense>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

struct Handles{
    size_t handleIndex[3];
    float newCoords[2];
    size_t newHandleIndex;
};

inline size_t findinarray(size_t * arr, size_t elem, size_t length){
    for(size_t i =0; i<length; i++){
        if(arr[i] == elem){
            return i;
        }
    }
    return length;
}

void deformMesh(std::vector<glm::vec3> * vertices,
                std::vector<unsigned int> * vertexIndices,
                Handles * handle,
                std::vector<std::vector<size_t> > * edgeNBH);

void MeshDeformStepOne(std::vector<glm::vec3> * vertices,
                       std::vector<unsigned int> * vertexIndices,
                       Handles * handle,
                       std::vector<glm::vec3> * intermediateVertices,
                       std::vector<Eigen::MatrixXf> * G_vector,
                       std::vector<std::vector<size_t> > * edgeNBH);

void MeshDeformStepTwo(std::vector<glm::vec3> * vertices,
                       std::vector<unsigned int> * vertexIndices,
                       Handles * handle,
                       std::vector<glm::vec3> * intermediateVertices,
                       std::vector<Eigen::MatrixXf> * G_vector,
                       std::vector<std::vector<size_t> > * edgeNBH);
                       
void getEdgeNeighbourHoods(std::vector<unsigned int> * vertexIndices,
                           std::vector<std::vector<size_t> > * edgeNBH);

#endif /* meshdeform_hpp */
