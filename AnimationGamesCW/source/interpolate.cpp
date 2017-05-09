#include "interpolate.hpp"

void MeshInterpolate::ComputeInitialMatricies(void){
    Eigen::MatrixXf M = Eigen::MatrixXf::Zero(6,6);
    Eigen::VectorXf q(6);
    Eigen::MatrixXf A_temp(2,2);
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(4*vertexIndices.size()/3,2*mesh_a.size()-2);

    for(size_t i = 0; i<vertexIndices.size(); i+=3){
        for(int j = 0; j<3; j++){
            M(2*j,0) = mesh_a[vertexIndices[i+j]].x;
            M(2*j,1) = mesh_a[vertexIndices[i+j]].y;
            M(2*j,2) = 1;
            M(2*j+1,3) = mesh_a[vertexIndices[i+j]].x;
            M(2*j+1,4) = mesh_a[vertexIndices[i+j]].y;
            M(2*j+1,5) = 1;
            q(2*j) = mesh_b[vertexIndices[i+j]].x;
            q(2*j+1) = mesh_b[vertexIndices[i+j]].y;
        }
        Eigen::MatrixXf P_temp = ((M.transpose()*M).inverse()) * M.transpose();
        Eigen::MatrixXf P_i = Eigen::MatrixXf::Zero(4,6);
        for(int j=0; j<4; j++){
            for(int k = 0; k<6; k++){
                P_i(j,k) = P_temp(j+(j>1),k);
            }
        }
        P_inverse[i/3] = P_i;
        Eigen::VectorXf Al = P_temp * q;
        A_temp << Al(0),Al(1),
             Al(3),Al(4);
        A_gamma[i/3] = A_temp;
        for(int j = 0; j<4; j++){
            for(int k = 0; k<3; j++){
                if(vertexIndices[i+k] > 0){
                    A(4*(i/3) + j, 2*vertexIndices[i+k]-2) =P_i(j,2*k);
                    A(4*(i/3) + j, 2*vertexIndices[i+k]-1) =P_i(j,2*k+1);
                }
            }
        }
    }
    A_inv = ((A.transpose()*A).inverse()) * A.transpose();
}

std::vector<glm::vec3> MeshInterpolate::Interpolate(float t){
    std::vector<glm::vec3> NewVertices = std::vector<glm::vec3>(mesh_a.size());
    NewVertices[0].x = (1.0f-t) * mesh_a[0].x + t*mesh_b[0].x;
    NewVertices[0].y = (1.0f-t) * mesh_a[0].y + t*mesh_b[0].y;
    NewVertices[0].z = mesh_a[0].z;
    Eigen::VectorXf b(4*vertexIndices.size()/3);
    for(size_t i = 0; i< vertexIndices.size(); i+=3){
        Eigen::MatrixXf A_out = SingleTriangleInterpolation(A_gamma[i/3],t);
        for(int j = 0; j<4; j++){
            b(i/3 + j) = A_out(j/2,j%2);
        }
        bool foundZero = 0;
        int foundZeroIndex;
        for(int j = 0; j<3; j++){
            if(vertexIndices[i+j]==0){
                foundZero = 1;
                foundZeroIndex = j;
                break;
            }
        }
        if(foundZero){
            Eigen::MatrixXf P_temp = P_inverse[i/3];
            for(int j = 0; j<4; j++){
                b(i/3 + j) -= P_temp(j,2*foundZeroIndex)*NewVertices[0].x + P_temp(j,2*foundZeroIndex + 1)*NewVertices[0].y;
            }
        }
    }
    Eigen::VectorXf newV = A_inv*b;
    for(size_t i = 1; i<mesh_a.size(); i++){
        NewVertices[i].x = newV(2*i -2);
        NewVertices[i].y = newV(2*i -1);
        NewVertices[i].z = mesh_a[0].z;
    }
    return NewVertices;
}

Eigen::MatrixXf MeshInterpolate::SingleTriangleInterpolation(Eigen::MatrixXf a, float t){
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(a,Eigen::ComputeFullU|Eigen::ComputeFullV);
    Eigen::VectorXf Singular = svd.singularValues();
    Eigen::MatrixXf Ry = svd.matrixU() * (svd.matrixV()).transpose();
    glm::mat3 Ry_temp(1.0);
    Ry_temp[0][0] =Ry(0,0);
    Ry_temp[1][0] =Ry(0,1);
    Ry_temp[0][1] =Ry(1,0);
    Ry_temp[1][1] =Ry(1,1);
    
    Eigen::MatrixXf S_diag(2,2);
    S_diag<<Singular(0),0,
            0,Singular(1);
    Eigen::MatrixXf S_gamma = svd.matrixV() * S_diag * (svd.matrixV()).transpose();

    glm::fquat q0 = {1.0f,0.0f,0.0f,0.0f};
    glm::fquat q1 = glm::quat_cast(Ry_temp);

    glm::fquat q_interp = glm::mix(q0,q1,t);

    glm::mat3 r_mat = glm::mat3_cast(q_interp);

    Eigen::MatrixXf R_interp(2,2);
    R_interp<<r_mat[0][0],r_mat[1][0],
              r_mat[0][1],r_mat[1][1];
    
    return R_interp * ((1.0f-t)*(Eigen::MatrixXf(2,2).setIdentity()) + t*S_diag);
}