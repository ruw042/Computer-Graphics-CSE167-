//
//  PlantGenerator.hpp
//  HW5
//
//  Created by Daniel Brim on 6/2/16.
//  Copyright © 2016 Daniel Brim. All rights reserved.
//

#ifndef PlantGenerator_hpp
#define PlantGenerator_hpp

#include "MatrixTransform.hpp"
//#include "Cylinder.hpp"
//#include "hw3_globals.h"
#include <string>
#include <vector>
#include "LSystem.hpp"

#include "OBJObject.h"

MatrixTransform *generatePlant(Grammar grammar, OBJObject *cylinder);

#endif /* PlantGenerator_hpp */
