//
//  meshdeform.cpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 08/03/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "meshdeform.hpp"
#include <cmath>

void deformMesh(std::vector<glm::vec3> *vertices, std::vector<unsigned int> * vertexIndices, Handles * handle,std::vector<std::vector<size_t> > * edgeNBH){
    std::vector<glm::vec3> intermediateVertices = std::vector<glm::vec3>(vertices->size());
    std::vector<Eigen::MatrixXf> G_vector;
    MeshDeformStepOne(vertices, vertexIndices, handle, &intermediateVertices, &G_vector, edgeNBH);
    //modifies vertices in place in step 2.
    MeshDeformStepTwo(vertices, vertexIndices, handle, &intermediateVertices, &G_vector, edgeNBH);
}

void MeshDeformStepOne(
                        std::vector<glm::vec3> * vertices,
                        std::vector<unsigned int> * vertexIndices,
                        Handles * handle,
                        std::vector<glm::vec3> * intermediateVertices,
                        std::vector<Eigen::MatrixXf> * G_vector,
                        std::vector<std::vector<size_t> > * edgeNBH
                        ){
    
    int w = 1000;
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(vertexIndices->size() * 2 + 6,vertices->size() * 2);
    Eigen::VectorXf B = Eigen::VectorXf::Zero(vertexIndices->size() * 2 + 6);
    Eigen::MatrixXf Edge_8(2,8);
    Eigen::MatrixXf Edge_6(2,6);
    Eigen::MatrixXf G_8(8,4);
    Eigen::MatrixXf G_inv_8(8,4);
    Eigen::MatrixXf G_6(6,4);
    Eigen::MatrixXf G_inv_6(6,4);
    Eigen::MatrixXf H_8(2,8);
    Eigen::MatrixXf H_6(2,6);
    Eigen::MatrixXf EdgeMat(2,2);
    Eigen::MatrixXf h_8(2,8);
    Eigen::MatrixXf h_6(2,6);

    Edge_6<<
    -1,0,1,0, 0,0,
    0,-1,0,1, 0,0;
    Edge_8<< 
    -1,0,1,0, 0,0,0,0,
    0,-1,0,1, 0,0,0,0;

    *G_vector = std::vector<Eigen::MatrixXf>(vertexIndices->size());
    for(size_t i = 0; i<vertexIndices->size(); i++){
        size_t vr = (*edgeNBH)[i][3];
        
        size_t vi = (*edgeNBH)[i][0];
        size_t vj = (*edgeNBH)[i][1];
        if(vr == vertexIndices->size()){
            for(int k = 0; k<3; k++){
                G_6(2*k,0) = vertices[0][(*edgeNBH)[i][k]].x;
                G_6(2*k,1) = vertices[0][(*edgeNBH)[i][k]].y;
                G_6(2*k,2) = 1;
                G_6(2*k,3) = 0;
                G_6(2*k+1,0) = vertices[0][(*edgeNBH)[i][k]].y;
                G_6(2*k+1,1) = -1*vertices[0][(*edgeNBH)[i][k]].x;
                G_6(2*k+1,2) = 0;
                G_6(2*k+1,3) = 1;
            }
            G_inv_6 = ((G_6.transpose()*G_6).inverse())*G_6.transpose();
            (*G_vector)[i] = G_inv_6;
        }
        else{
            for(int k = 0; k<4; k++){
                G_8(2*k,0) = vertices[0][(*edgeNBH)[i][k]].x;
                G_8(2*k,1) = vertices[0][(*edgeNBH)[i][k]].y;
                G_8(2*k,2) = 1;
                G_8(2*k,3) = 0;
                G_8(2*k+1,0) = vertices[0][(*edgeNBH)[i][k]].y;
                G_8(2*k+1,1) = -1*vertices[0][(*edgeNBH)[i][k]].x;
                G_8(2*k+1,2) = 0;
                G_8(2*k+1,3) = 1;
            }
            G_inv_8 = ((G_8.transpose()*G_8).inverse())*G_8.transpose();
            (*G_vector)[i] = G_inv_8;
        }

        float edge_vij[2] = {vertices[0][vj].x - vertices[0][vi].x,vertices[0][vj].y - vertices[0][vi].y};
        EdgeMat<<edge_vij[0],edge_vij[1],
                edge_vij[1],-1*edge_vij[0];
        
        if(vr == vertexIndices->size()){
            for(auto k =0; k<6; k++){
                H_6(0,k) = G_inv_6(0,k);
                H_6(1,k) = G_inv_6(1,k);
            }
           h_6 = Edge_6 - EdgeMat*H_6;
            for(auto k = 0; k<3; k++){
                A(2*i, 2*(*edgeNBH)[i][k]) =   h_6(0,2*k);
                A(2*i, 2*(*edgeNBH)[i][k]+1) = h_6(0,2*k+1);
                A(2*i+1, 2*(*edgeNBH)[i][k]) = h_6(1,2*k);
                A(2*i+1, 2*(*edgeNBH)[i][k]+1)=h_6(1,2*k+1); 
            }
        }
        else{
            for(auto k =0; k<8; k++){
                H_8(0,k) = G_inv_8(0,k);
                H_8(1,k) = G_inv_8(1,k);
            }
            h_8 = Edge_8 - EdgeMat*H_8;
            for(auto k = 0; k<4; k++){
                A(2*i, 2*(*edgeNBH)[i][k]) =   h_8(0,2*k);
                A(2*i, 2*(*edgeNBH)[i][k]+1) = h_8(0,2*k+1);
                A(2*i+1, 2*(*edgeNBH)[i][k]) = h_8(1,2*k);
                A(2*i+1, 2*(*edgeNBH)[i][k]+1)=h_8(1,2*k+1); 
            }
        }

    }
    //end of iterating loop.
    for(int i = 0; i<3; i++){
        if(i == handle->newHandleIndex){
            B[vertexIndices->size() * 2 + 2*i] = w*handle->newCoords[0];
            B[vertexIndices->size() * 2 + 2*i + 1] = w*handle->newCoords[1];
        }
        else{
            B[vertexIndices->size() * 2 + 2*i] = w*vertices[0][handle->handleIndex[i]].x;
            B[vertexIndices->size() * 2 + 2*i + 1] = w*vertices[0][handle->handleIndex[i]].y;
        }
        A(vertexIndices->size() * 2 + 2*i,2*handle->handleIndex[i]) = w;
        A(vertexIndices->size() * 2 + 2*i+1,2*handle->handleIndex[i]+1) = w;
    }

    Eigen::VectorXf newV = (A.transpose() * A).llt().solve(A.transpose()*B);
    for(size_t i = 0; i < vertices->size(); i++){
        intermediateVertices[0][i].x = newV[2*i];
        intermediateVertices[0][i].y = newV[2*i+1];
    }
}

void MeshDeformStepTwo(
std::vector<glm::vec3> * vertices,
std::vector<unsigned int> * vertexIndices,
Handles * handle,
std::vector<glm::vec3> * intermediateVertices,
std::vector<Eigen::MatrixXf> * G_vector,
std::vector<std::vector<size_t> > * edgeNBH
){
    int w = 1000;
    Eigen::MatrixXf A  = Eigen::MatrixXf::Zero(vertexIndices->size() + 3,vertices->size());
    Eigen::VectorXf Bx(vertexIndices->size() + 3);
    Eigen::VectorXf By(vertexIndices->size() + 3);
    Eigen::VectorXf V_NBH(8);
    Eigen::Matrix2f T;
    Eigen::Vector2f Edge;
    for(int i = 0; i<3; i++){
        if(i == handle->newHandleIndex){
            Bx[vertexIndices->size() +i] = w*handle->newCoords[0];
            By[vertexIndices->size() +i] = w*handle->newCoords[1];
        }     else{
            Bx[vertexIndices->size() +i] = w*vertices[0][handle->handleIndex[i]].x;
            By[vertexIndices->size() +i] = w*vertices[0][handle->handleIndex[i]].y;
        }
        A(vertexIndices->size() +i,handle->handleIndex[i]) = w;
    }
    for(size_t i=0; i<vertexIndices->size(); i++){
        size_t vr = (*edgeNBH)[i][3];
        int numIndices =4;
        if(vr == vertexIndices->size()){
            numIndices = 3;
            V_NBH.resize(6);
        }
        else{
            V_NBH.resize(8);
        }
        for(int j = 0; j<numIndices; j++){
            V_NBH(2*j) = intermediateVertices[0][(*edgeNBH)[i][j]].x;
            V_NBH(2*j+1) = intermediateVertices[0][(*edgeNBH)[i][j]].y;
        }
        Eigen::MatrixXf G = (*G_vector)[i];
        Eigen::VectorXf cs = G*V_NBH;
        float normaliser = sqrt(cs[0]*cs[0] + cs[1]*cs[1]);
        T<<
        cs[0]/normaliser,cs[1]/normaliser,
        -1*cs[1]/normaliser, cs[0]/normaliser;
        Edge<<
        vertices[0][(*edgeNBH)[i][1]].x - vertices[0][(*edgeNBH)[i][0]].x,
        vertices[0][(*edgeNBH)[i][1]].y - vertices[0][(*edgeNBH)[i][0]].y;
        Eigen::Vector2f Te_product = T*Edge;
        Bx[i] = Te_product[0];
        By[i] = Te_product[1];
        A(i,(*edgeNBH)[i][0]) = -1;
        A(i,(*edgeNBH)[i][1]) = 1;
    }

    Eigen::VectorXf newVx = (A.transpose() * A).llt().solve(A.transpose()*Bx);
    Eigen::VectorXf newVy = (A.transpose() * A).llt().solve(A.transpose()*By);
    for(size_t i = 0; i<vertices->size(); i++){
        vertices[0][i].x = newVx[i];
        vertices[0][i].y = newVy[i];
    }

}

int getEdgeNeighbourHoods(std::vector<unsigned int> * vertexIndices,std::vector<std::vector<size_t> > * edgeNBH){
    *edgeNBH = std::vector<std::vector<size_t> >(vertexIndices->size());
    size_t vi;
    size_t vj;
    size_t vl;

    for(size_t i = 0; i<vertexIndices->size(); i++){
        size_t vr = vertexIndices->size();
        vi = (*vertexIndices)[i];
        if((i%3)==2){
            vj = (*vertexIndices)[3*(i/3)];
            vl = (*vertexIndices)[3*(i/3) + 1];
        }
        else{
            vj = (*vertexIndices)[i+1];
            if((i%3)==1){
                vl = (*vertexIndices)[3*(i/3)];
            }
            else{
                vl = (*vertexIndices)[i+2];
            }
            
        }
        
        //linear search, leads to overall quadratic complexity in the search (worst case).
        //possible scope for improvement.
        for(size_t k = 0; k<vertexIndices->size(); k+=3){
            size_t searchLoc[3] = {(*vertexIndices)[k],(*vertexIndices)[k+1],(*vertexIndices)[k+2]};
            if(k==3*(i/3)){
                continue;
            }
            auto vi_location = findinarray(searchLoc, vi, 3);
            if(vi_location == 3){
                continue;
            }
            auto vj_location = findinarray(searchLoc, vj, 3);
            
            if(vj_location != 3){
                vr = searchLoc[3-vi_location-vj_location];
                if(vr > vertexIndices->size() ){
                    std::cerr<<"Garbage access of the search for the neighbour. Program will exit";
                    return EXIT_FAILURE;
                }
                break;
            }
        }
        (*edgeNBH)[i].push_back(vi);
        (*edgeNBH)[i].push_back(vj);
        (*edgeNBH)[i].push_back(vl);
        (*edgeNBH)[i].push_back(vr);
    }
    return EXIT_SUCCESS;
}
