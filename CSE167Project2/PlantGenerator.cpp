//
//  PlantGenerator.cpp
//  HW5
//
//  Created by Daniel Brim on 6/2/16.
//  Copyright © 2016 Daniel Brim. All rights reserved.
//

#include "PlantGenerator.hpp"
#include "Geode.hpp"

// FFFF-[[FF-[[F-[[X]+X]+F[+FX]-X]+F-[[X]+X]+F[+FX]-X]+FF[+FFF-[[X]+X]+F[+FX]-X]-F-[[X]+X]+F[+FX]-X]+FF-[[F-[[X]+X]+F[+FX]-
//variables : X F
//constants : + − [ ]
//start  : X
//rules  : (X → F−[[X]+X]+F[+FX]−X), (F → FF)
//angle  : 25°

struct Save {
    MatrixTransform *pos;
    int z_angle;
    int y_angle;
    int x_angle;
};

//extern OBJObject *cylinder;

MatrixTransform *generatePlant(Grammar grammar, OBJObject *cylinder) {
    std::string grammarStr = expandGrammerToLevel(grammar, grammar.levels);
        
    MatrixTransform *plant = new MatrixTransform(glm::mat4());
    
    MatrixTransform *mt_cyl = new MatrixTransform(glm::mat4());
    
//    Material chrome = {glm::vec3(0.39f,0.19f,0.0f),
//        glm::vec3(0.5,0.5,0.5),
//        glm::vec3(0.20, 0.20, 0.20),
//        10.0};
    
    Geode *g_cyl = new Geode(cylinder);
//    Cylinder *g_cyl = new Cylinder();
//    cylinder->material = chrome;
    
    g_cyl->toDraw = cylinder;
    g_cyl->update(glm::mat4(1.0f));
    
    mt_cyl->addChild(g_cyl);
    
    //    mt_cyl->rotate(90, glm::vec3(0,0,1));
    
//    std::vector<MatrixTransform *> *stack = new std::vector<MatrixTransform *>();
//    MatrixTransform *parent = plant;
    
    std::vector<Save> *stack = new std::vector<Save>();
    MatrixTransform *parent = plant;
    int z_angle = 0;
    int y_angle = 0;
    int x_angle = 0;

    
    float size_scale = 1.0f;
    int glevel = 1;
    glm::vec3 color = glm::vec3(0.39f,0.19f,0.0f); // brown
    for ( std::string::iterator it=grammarStr.begin(); it!=grammarStr.end(); ++it) {
        if (*it == 'F' || *it == 'Y') {
            // Add a Cylinder as a child at the end

            MatrixTransform *wrapper = new MatrixTransform(glm::mat4());
            MatrixTransform *direct_wrapper = new MatrixTransform(glm::mat4());

            Geode *g_cyl = new Geode(cylinder);
            OBJObject *cc = new OBJObject(cylinder->vertices, cylinder->normals, cylinder->indices);
            cc->material = chrome;
            cc->material.ambient = color;
            g_cyl->toDraw = cc;
            ((OBJObject *)(g_cyl->toDraw))->special = color;
            g_cyl->update(glm::mat4(1.0f));
            
            direct_wrapper->addChild(g_cyl);
//            if (glevel != 2) {
                direct_wrapper->scale(glm::vec3(0.5f*size_scale, grammar.f_length, 0.5f*size_scale));
//            } else {
//                direct_wrapper->scale(glm::vec3(0.5f*size_scale, 1.0f, 0.0f));
//            }
            wrapper->addChild(direct_wrapper);
//            if (glevel != 2) {
                wrapper->translate(glm::vec3(0.0f,grammar.f_length - 1,0.0f));
//            } else {
//                wrapper->translate(glm::vec3(0.0f,0.5,0.0f));
//            }
            wrapper->rotate(z_angle, glm::vec3(0.0f,0.0,1.0f));
            wrapper->rotate(grammar.y_angle, glm::vec3(0.0f,1.0,0.0f));
            
            parent->addChild(wrapper);
            parent = wrapper;
            z_angle = 0;
            y_angle = 0;
        } else if (*it == '+') {
            // turn left by ? degrees
            z_angle += grammar.z_angle;
        } else if (*it == '-') {
            // turn right by degrees
            z_angle += -grammar.z_angle;
        } else if (*it == '&') {
            // rotate around y axis by 20
            y_angle += grammar.y_angle;
        } else if (*it == '^') {
            // rotate around y axis by 20
            y_angle += -grammar.y_angle;
        } else if (*it == '[') {
            //The square bracket "[" corresponds to saving the current values for position and angle, which are restored when the corresponding "]" is executed.
            //save current position? and angle
            Save s = {parent, z_angle,y_angle,x_angle};
            stack->push_back(s);

        } else if (*it == ']') {
            // pop state off stack
            Save s = stack->back();
            stack->pop_back();
            parent = s.pos;
            z_angle = s.z_angle;
            x_angle = s.x_angle;
            
        } else if (*it == '1' || *it == '2' || *it == '3') {
            // pop state off stack
            glevel = (int)(*it - 48);
            if (glevel == 1) {
                color = glm::vec3(0.39f,0.19f,0.0f); // brown
            } else {
                float green = 1 - 1.0f / glevel;
                float r = green / 3.0f;
                float b = r;
                color = glm::vec3(r,green,b);
            }
//            std::cout << glevel;
            size_scale = 1.0f / glevel;
//            printf("%f \n", glevel);
            
        }
    }
    
//    plant->scale(glm::vec3(0.25f, 2.5f, 0.25f));
    
    return plant;

}
