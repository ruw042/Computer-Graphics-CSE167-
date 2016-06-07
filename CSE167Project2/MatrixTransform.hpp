//
//  MatrixTransform.hpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#ifndef MatrixTransform_hpp
#define MatrixTransform_hpp

#include <stdio.h>
#include <list>
#include "Group.hpp"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


class MatrixTransform : public Group
{
public:
    MatrixTransform(glm::mat4 mat);
    void update(glm::mat4 C);
    glm::mat4 MT = glm::mat4(1.0f);
};




#endif /* MatrixTransform_hpp */
