#pragma once

#ifndef KITTY_UTILITY_HPP
#define KITTY_UTILITY_HPP

#include <kitty/kitty_stl_impl.hpp>

#include <sstream>
#include <string>

namespace kitty {

using std::istringstream;
using std::string;

class kitty_utility {

public:
    static int string_to_int(string const & expression) {
        if (expression.empty()) {
            return 0;
        }
        istringstream iss(expression);
        auto isNegativeValue = false;
        if (iss.str()[0] == '-') {
            isNegativeValue = true;
            char c;
            iss >> c;
        }
        auto value = 0;
        iss >> value;
        if (isNegativeValue) {
            value *= -1;
        }
        return value;
    }

    static string get_line() {
        string line;
        while (1) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n') {
                    break;
                }
                line += c;
            }
        }
        if (line[line.size() - 1] == ' ') {
            line.erase(line.size() - 1);
        }
        return line;
    }

private:

protected:

};

} // kitty

#endif // KITTY_UTILITY_HPP
