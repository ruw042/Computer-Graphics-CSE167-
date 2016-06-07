//
//  LSystem.hpp
//  HW5
//
//  Created by Daniel Brim on 6/2/16.
//  Copyright © 2016 Daniel Brim. All rights reserved.
//

#ifndef LSystem_hpp
#define LSystem_hpp

#include <stdio.h>
#include <string>
#include <vector>

// grammar rules
// (X → F-[[X]+X]+F[+FX]-X),(F → FF)
// start: X
// F - move forward
//
// Constants: +, −
// Turn left, right


struct Grammar {
    std::vector<std::string> producers;
    std::vector<std::string> rules;
    std::string start;
    float f_length;
    int levels;
    int z_angle;
    int y_angle;
};

std::string expandGrammerToLevel(Grammar grammar, int level);

#endif /* LSystem_hpp */
