#ifndef interpolate_hpp
#define interpolate_hpp

#include <vector>
#include <cmath>

#include <Eigen/Dense>
#include <Eigen/SVD>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class MeshInterpolate{
    public:
    MeshInterpolate(std::vector<glm::vec3> mesh_a, std::vector<glm::vec3> mesh_b, std::vector<unsigned int> vertexIndices){
        this->mesh_a = mesh_a;
        this->mesh_b = mesh_b;
        this->vertexIndices = vertexIndices;
        P_inverse = std::vector<Eigen::MatrixXf>(vertexIndices.size()/3);
        A_gamma = std::vector<Eigen::MatrixXf>(vertexIndices.size()/3);
    }
    void ComputeInitialMatricies(void);
    std::vector<glm::vec3> Interpolate(float t);

    private:
    Eigen::MatrixXf A_inv;
    std::vector<Eigen::MatrixXf> P_inverse, A_gamma;
    std::vector<glm::vec3> mesh_a, std::vector<glm::vec3> mesh_b;
    std::vector<unsigned int> vertexIndices;
    Eigen::MatrixXf SingleTriangleInterpolation(Eigen::MatrixXf a, float t);
}

#endif