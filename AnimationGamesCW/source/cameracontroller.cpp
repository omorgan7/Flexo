//
//  cameracontroller.cpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 24/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#include "cameracontroller.hpp"

void computeScalingMatrix(int window_width, int window_height, glm::mat4 * ProjectionMatrix ){
    
    
    float FoV = 90.0f;
    float aspectRatio = (float)window_width/(float)window_height;
    
    // Projection matrix : 45∞ Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
    *ProjectionMatrix = glm::perspective(glm::radians(FoV), aspectRatio, 0.001f, 100.0f);
    // Camera matrix

}
