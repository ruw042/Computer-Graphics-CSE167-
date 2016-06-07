//
//  Geode.cpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#include "Geode.hpp"
#include "Window.h"

extern GLint shaderProgram;

Geode::Geode(OBJObject * obj)
{
    toDraw = obj;
}
void Geode::update(glm::mat4 C)
{
    M = C;
}
void Geode::draw()
{
    toDraw->draw(shaderProgram);
}
