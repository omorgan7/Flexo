//
//  meshdeform.cpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 08/03/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "meshdeform.hpp"
#include <fstream>
#include <cmath>

void deformMesh(std::vector<glm::vec3> *vertices, std::vector<unsigned int> * vertexIndices, Handles * handle){
    std::vector<glm::vec3> intermediateVertices = std::vector<glm::vec3>(vertices->size());
    MeshDeformStepOne(vertices, vertexIndices, handle, &intermediateVertices);
    for(size_t i = 0; i<vertices->size(); i++){
        vertices[0][i].x = intermediateVertices[i].x;
        vertices[0][i].y = intermediateVertices[i].y;
    }
    //modifies vertices in place in step 2.
    //MeshDeformStepTwo(vertices, vertexIndices, handle, &intermediateVertices);
}

inline size_t findinarray(size_t * arr, size_t elem, size_t length){
    for(size_t i =0; i<length; i++){
        if(arr[i] == elem){
            return i;
        }
    }
    return length;
}

void MeshDeformStepOne(std::vector<glm::vec3> * vertices, std::vector<unsigned int> * vertexIndices, Handles * handle,std::vector<glm::vec3> * intermediateVertices){
    
    int w = 1000;
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(vertexIndices->size() * 2 + 6,vertices->size() * 2);
    Eigen::VectorXf B = Eigen::VectorXf::Zero(vertexIndices->size() * 2 + 6);
    Eigen::MatrixXf Edge(2,8);
    Eigen::MatrixXf G(8,4);
    Eigen::MatrixXf H(2,8);
    Eigen::MatrixXf EdgeMat(2,2);
    size_t vi;
    size_t vj;
    size_t vl;
    size_t vertex_nbh[4];
    
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
                        std::cout<<"oh no!\n";
                    }
                    break;

                }
            }
            auto v_nbh_length = 4;
            vertex_nbh[0] = vi;
            vertex_nbh[1] = vj;
            vertex_nbh[2] = vl;
            vertex_nbh[3] = vr;
            if(vr == vertexIndices->size()){
                v_nbh_length = 3;
                Edge.resize(2,6);
                G.resize(6,4);
                H.resize(2,6);
                Edge<< -1,0,1,0, 0,0,
                0,-1,0,1, 0,0;
            }
            else{
                Edge.resize(2,8);
                G.resize(8,4);
                H.resize(2,8);
                Edge<< -1,0,1,0, 0,0,0,0,
                0,-1,0,1, 0,0,0,0;
                G(6,0) = vertices[0][vr].x;
                G(6,1) = vertices[0][vr].y;
                G(6,2) = 1;
                G(6,3) = 0;
                G(7,0) = vertices[0][vr].y;
                G(7,1) = -1*vertices[0][vr].x;
                G(7,2) = 0;
                G(7,3) = 1;
            }
            for(int k = 0; k<3; k++){
                G(2*k,0) = vertices[0][vertex_nbh[k]].x;
                G(2*k,1) = vertices[0][vertex_nbh[k]].y;
                G(2*k,2) = 1;
                G(2*k,3) = 0;
                G(2*k+1,0) = vertices[0][vertex_nbh[k]].y;
                G(2*k+1,1) = -1*vertices[0][vertex_nbh[k]].x;
                G(2*k+1,2) = 0;
                G(2*k+1,3) = 1;
            }

            //std::cout<<G<<"\n";
            Eigen::MatrixXf G_inv_product = ((G.transpose()*G).inverse())*G.transpose();
            for(auto k =0; k<2*v_nbh_length; k++){
                H(0,k) = G_inv_product(0,k);
                H(1,k) = G_inv_product(1,k);
            }
        
            float edge_vij[2] = {vertices[0][vj].x - vertices[0][vi].x,vertices[0][vj].y - vertices[0][vi].y};
            EdgeMat<<edge_vij[0],edge_vij[1],
                    edge_vij[1],-1*edge_vij[0];
        
            Eigen::MatrixXf h = Edge - EdgeMat*H;

            for(auto k = 0; k<v_nbh_length; k++){
                A(2*i, 2*vertex_nbh[k]) = h(0,2*k);
                A(2*i, 2*vertex_nbh[k]+1) = h(0,2*k+1);
                A(2*i+1, 2*vertex_nbh[k]) = h(1,2*k);
                A(2*i+1, 2*vertex_nbh[k]+1) = h(1,2*k+1);
                if(isnan(h(0,2*k)) || isnan(h(0,2*k+1)) || isnan(h(1,2*k)) || isnan(h(1,2*k+1))){
                    std::cout<<G<<"\n";
                    std::cout<<G_inv_product<<"\n";
                    std::cout<<h<<"\n";
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
    //std::cout<<B<<"\n";
    Eigen::VectorXf newV = (A.transpose() * A).inverse()*A.transpose()*B;

    for(size_t i = 0; i < vertices->size(); i++){
        intermediateVertices[0][i].x = newV[2*i];//*0.35;
        intermediateVertices[0][i].y = newV[2*i+1];//*0.35;
    }
    
    
}


