//
//  Node.hpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/3/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


class Node
{
public:
    virtual void draw() = 0;
    virtual void update(glm::mat4 C) = 0;
};

#endif /* Node_hpp */
