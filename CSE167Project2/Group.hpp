//
//  Group.hpp
//  CSE167Project2
//
//  Created by Runping Wang on 5/4/16.
//  Copyright © 2016 Runping Wang. All rights reserved.
//

#ifndef Group_hpp
#define Group_hpp

#include <stdio.h>
#include <list>
#include "Node.hpp"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

//using NodePtr = std::shared_ptr<Node>;

class Group : public Node
{
public:
    void addChild(Node* nodePtr);
    void removeChild(Node* nodePtr);
    virtual void draw();
    virtual void update(glm::mat4 matrix);
    std::list<Node*> children;
};






#endif /* Group_hpp */
