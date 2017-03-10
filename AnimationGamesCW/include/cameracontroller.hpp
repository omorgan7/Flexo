//
//  cameracontroller.hpp
//  AnimationGamesCW
//
//  Created by Owen Morgan on 24/02/2017.
//  Copyright © 2017 Owen Morgan. All rights reserved.
//

#ifndef cameracontroller_hpp
#define cameracontroller_hpp

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void computeScalingMatrix(int window_width, int window_height, glm::mat4 * ProjectionMatrix );

#endif /* cameracontroller_hpp */
