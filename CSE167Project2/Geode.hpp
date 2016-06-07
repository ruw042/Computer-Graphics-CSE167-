//
//  Geode.hpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#ifndef Geode_hpp
#define Geode_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "Node.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "OBJObject.h"

class Geode : public Node
{
    public:
        Geode(OBJObject * obj);
        OBJObject *toDraw;
        glm::mat4 M = glm::mat4(1.0f);
        void update(glm::mat4 C);
        void draw();
};




#endif /* Geode_hpp */
