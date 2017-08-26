#pragma once

#ifndef KITTY_SYMBOLS_HPP_
#define KITTY_SYMBOLS_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>

namespace kitty {

using std::string;    

class kitty_symbols {

public:
    static bool command_contains_invalid_character(string const & command) {
        MatchState matchState;
        matchState.Target(command.c_str());
        if (matchState.Match("([^%a%d%+%-%*%/%(%)\"\'!@#$%%^&_ ]+)") > 0) {
            Serial.print(F("ERROR: invalid character"));
            if (matchState.MatchCount("[^%a%d%+%-%(%)_ ]") > 0) {
                Serial.print(F("s"));
            }
            Serial.println(F(""));
            matchState.GlobalMatch("[^%a%d%+%-%(%)_ ]", 
                [] (const char * match, unsigned int const length, MatchState const & ms) {
                    Serial.print(string(match, length).c_str());
                    Serial.print(" ");
                });
            Serial.println(F(""));
            return true;
        }
        return false;
    }

private:

protected:

};

}

#endif // KITTY_SYMBOLS_HPP_