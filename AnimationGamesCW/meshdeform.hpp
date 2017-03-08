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

void deformMesh(std::vector<glm::vec3> * vertices,
                std::vector<unsigned int> * vertexIndices,
                Handles * handle);

void MeshDeformStepOne(std::vector<glm::vec3> * vertices,
                       std::vector<unsigned int> * vertexIndices,
                       Handles * handle,
                       std::vector<glm::vec3> * intermediateVertices);

void MeshDeformStepTwo(std::vector<glm::vec3> * vertices,
                       std::vector<unsigned int> * vertexIndices,
                       Handles * handle,
                       std::vector<glm::vec3> * intermediateVertices);

#endif /* meshdeform_hpp */
