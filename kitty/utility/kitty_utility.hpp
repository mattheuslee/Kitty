#pragma once

#ifndef KITTY_UTILITY_HPP
#define KITTY_UTILITY_HPP

#include <StandardCplusplus.h>

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

private:

protected:

};

} // kitty

#endif // KITTY_UTILITY_HPP
