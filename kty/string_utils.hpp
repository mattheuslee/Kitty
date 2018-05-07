#pragma once

#include <kty/stl_impl.hpp>
#include <cctype>
#include <sstream>
#include <string>
#include <utility>

#include <kty/containers/string.hpp>

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
    @brief  Converts an integer into its string representation.

    @param  i
            The integer to be converted.

    @param  stringPool
            The pool used to allocate strings.

    @return The string representation of the integer.
*/
template <typename StringPool>
PoolString<StringPool> int_to_str(int i, StringPool & stringPool) {
    char strChar[2] = "";
    bool isNegative = false;
    if (i < 0) {
        isNegative = true;
        i *= -1;
    }
    PoolString<StringPool> output(stringPool);
    // output will contain digits in reverse order
    while (i > 0) {
        strChar[0] = (char)(i % 10 + '0');
        output += strChar;
        i /= 10;
    }
    // Reverse digits to correct order
    int len = output.strlen();
    char* cstr = output.c_str();
    for (int j = 0; j < len / 2; ++j) {
        std::swap(cstr[j], cstr[len - j - 1]);
    }
    // Add negative sign if necessary
    if (isNegative) {
        for (int j = len + 1; j > 0; --j) {
            cstr[j] = cstr[j - 1];            
        }
        cstr[0] = '-';
    }
    return output;
}

/*! 
    @brief  Removes all whitespace characters from the string.
            This function modifies the string parameter in place.

    @param  str
            The string to remove whitespace from.
*/
void remove_str_whitespace(std::string & str) {
    std::string::iterator i = str.begin();
    while (i != str.end()) {
        if (isspace(*i)) {
            i = str.erase(i);
        }
        else {
            ++i;
        }
    }
}

/*! 
    @brief  Removes all whitespace characters from the string.
            This function modifies the string parameter in place.

    @param  str
            The string to remove whitespace from.
*/
template <typename StringPool>
void remove_str_whitespace(PoolString<StringPool> & str) {
    PoolString<StringPool> temp(str);
    temp = "";
    char str_[2] = " ";
    int len = str.strlen();
    for (int i = 0; i < len; ++i) {
        if (!isspace(str.c_str()[i])) {
            str_[0] = str.c_str()[i];
            temp += str_;
        }
    }
    str = temp;
}

} // namespace kty
