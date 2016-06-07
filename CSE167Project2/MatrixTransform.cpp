//
//  MatrixTransform.cpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#include "MatrixTransform.hpp"

//MatrixTransform::MatrixTransform(glm::mat4 mat)
//{
//    Group::update(mat*MT);
//}

void MatrixTransform::update(glm::mat4 C)
{
    Group::update(C*MT);
}

void MatrixTransform::rotate(float angle, glm::vec3 axis) {
    MT = glm::rotate(glm::mat4(1.0f), angle/180.0f * glm::pi<float>(), axis) * MT;
}

void MatrixTransform::scale(glm::vec3 scale) {
    glm::mat4 res =  glm::scale(glm::mat4(1), scale);
    MT = res * MT;
}

void MatrixTransform::translate(glm::vec3 amount) {
    glm::mat4 t = glm::translate(glm::mat4(), amount);
    MT = t * MT;
}
