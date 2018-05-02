#pragma once

#include <kty/stl_impl.hpp>
#include <cctype>
#include <sstream>
#include <string>
#include <ArduinoLog.h>

namespace kty {

/*! 
    @brief  Converts the string representation of an integer to an int.

    @param  str
            The string to be converted.
            The string can be prefixed with a '-', which indicates a negative number.

    @return The int represented in the string.
            If the string is invalid, 0 is returned.
*/
int str_to_int(std::string const & str) {
    if (str.empty()) {
        return 0;
    }
    for (int i = 0; i < str.size(); ++i) {
        if (!isdigit(str[i]) && str[i] != '-') {
            return 0;
        }
    }
    std::istringstream iss(str);
    bool isNegativeValue = false;
    if (iss.str()[0] == '-') {
        isNegativeValue = true;
        char c;
        iss >> c; // Eat '-'
    }
    int value = 0;
    iss >> value;
    if (isNegativeValue) {
        value *= -1;
    }
    return value;
}
/*! 
    @brief  Converts an integer into its string representation.

    @param  i
            The integer to be converted.

    @return The string representation of the integer.
*/
std::string int_to_str(int const & i) {
    std::ostringstream oss;
    oss << i;
    return oss.str();
}

/*! 
    @brief  Removes all whitespace characters from the string.

    @param  str
            The string to remove whitespace from.

    @return The input string with all whitespace removed.
*/
std::string remove_str_whitespace(std::string const & str) {
    std::string result;
    for (int i = 0; i < str.size(); ++i) {
        if (!isspace(str[i])) {
            result += str[i];
        }
    }
    return result;
}

} // namespace kty
