//
//  LSystem.cpp
//  HW5
//
//  Created by Daniel Brim on 6/2/16.
//  Copyright © 2016 Daniel Brim. All rights reserved.
//

#include "LSystem.hpp"
#include <iostream>

std::string testStr = "FFFF-[[FF-[[F-[[X]+X]+F[+FX]-X]+F-[[X]+X]+F[+FX]-X]+FF[+FFF-[[X]+X]+F[+FX]-X]-F-[[X]+X]+F[+FX]-X]+FF-[[F-[[X]+X]+F[+FX]-X]+F-[[X]+X]+F[+FX]-X]+FF[+FFF-[[X]+X]+F[+FX]-X]-F-[[X]+X]+F[+FX]-X]+FFFF[+FFFFFF-[[F-[[X]+X]+F[+FX]-X]+F-[[X]+X]+F[+FX]-X]+FF[+FFF-[[X]+X]+F[+FX]-X]-F-[[X]+X]+F[+FX]-X]-FF-[[F-[[X]+X]+F[+FX]-X]+F-[[X]+X]+F[+FX]-X]+FF[+FFF-[[X]+X]+F[+FX]-X]-F-[[X]+X]+F[+FX]-X";


std::string replaceCharacterAtIndexWith(std::string ogString, int index, std::string replaceString);

std::string expandGrammerToLevel(Grammar grammar, int level) {
//    return "FF-[[F-[[&X]+^X]+F[+F&X]-^X]+F-[[&X]+^X]+F[+F&X]-^X]+FF[+FFF-[[&X]+^X]+F[+F&X]-^X]-F-[[&X]+^X]+F[+F&X]-^X";
    
    
    // (F)FXX
    // F->FFXX
    // X->FFXX
    
    std::string expandme = grammar.start;
    std::string expanded = "";
    for (int i = 0; i < grammar.levels; i++) {
        for ( std::string::iterator it=expandme.begin(); it!=expandme.end(); ++it) {
            // iterate over string, char by char
            char c = *it;
            std::string str(1,c);

            std::vector<std::string> v;
            std::vector<std::string>::iterator finder;

            finder = std::find(grammar.producers.begin(), grammar.producers.end(), str);
            if (finder != grammar.producers.end()) {
                // character is a producer
                // so find the rule for it
                int rule_index = (int)(finder - grammar.producers.begin());
                std::string rule = grammar.rules[rule_index];
                expanded += rule;
            } else{
                // character is not a producer, so just append it
                expanded += *it;
            }
        }
        expandme = expanded;
        expanded = "";
    }
    return expandme;

}

std::string replaceCharacterAtIndexWith(std::string ogString, int index, std::string replaceString) {
    ogString.replace(index, 1, replaceString);
    return ogString;
}

