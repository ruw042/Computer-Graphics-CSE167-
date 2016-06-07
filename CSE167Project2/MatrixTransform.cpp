//
//  MatrixTransform.cpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#include "MatrixTransform.hpp"

MatrixTransform::MatrixTransform(glm::mat4 mat)
{
    MT = mat;
}

void MatrixTransform::update(glm::mat4 C)
{
    glm::mat4 temp = C*MT;
    Group::update(temp);
}
