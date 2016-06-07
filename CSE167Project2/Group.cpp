//
//  Group.cpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#include "Group.hpp"

void Group::addChild(Node* nodePtr)
{
    children.push_back(nodePtr);
}

void Group::removeChild(Node* nodePtr)
{
    children.remove(nodePtr);
}

void Group::draw()
{
    for (std::list<Node*>::iterator it=children.begin(); it!=children.end(); ++it)
        (*it)->draw();
}
void Group::update(glm::mat4 matrix)
{
    for (std::list<Node*>::iterator it=children.begin(); it!=children.end(); ++it)
        (*it)->update(matrix);
}

