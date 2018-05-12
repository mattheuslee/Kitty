#pragma once

#include <kty/stl_impl.hpp>
#include <cctype>

#include <kty/containers/string.hpp>
#include <kty/types.hpp>

namespace kty {

/*! 
    @brief  Converts the string representation of an integer to an int.

    @param  str
            The string to be converted.
            The string can be prefixed with a '-', which indicates a negative number.

    @return The int represented in the string.
            If the string is invalid, 0 is returned.
*/
template <typename StringPool>
int str_to_int(PoolString<StringPool> const & str) {
    Log.verbose(F("%s\n"), PRINT_FUNC);
    int len = str.strlen();
    if (len == 0) {
        return 0;
    }
    if (!isdigit(str[0]) && str[0] != '-') {
        return 0;
    }
    for (int i = 1; i < len; ++i) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    int result = 0;
    bool isNegative = false;
    int startIdx = 0;
    if (str[0] == '-') {
        isNegative = true;
        startIdx = 1;
    }
    for (int i = startIdx; i < len; ++i) {
        result *= 10;
        result += (int)(str[i] - '0');
    }
    if (isNegative) {
        result *= -1;
    }
    return result;
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
    Log.verbose(F("%s\n"), PRINT_FUNC);
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
template <typename StringPool>
void remove_str_whitespace(PoolString<StringPool> & str) {
    Log.verbose(F("%s\n"), PRINT_FUNC);
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

/*! 
    @brief  Removes multiple whitespace characters from the string,
            replacing them with a single whitespace character.
            This function modifies the string parameter in place.

    @param  str
            The string to remove whitespace from.
*/
template <typename StringPool>
void remove_str_multiple_whitespace(PoolString<StringPool> & str) {
    Log.verbose(F("%s\n"), PRINT_FUNC);
    PoolString<StringPool> temp(str);
    temp = "";
    char str_[2] = " ";
    int len = str.strlen();
    int i = 0;
    while (i < len) {
        str_[0] = str[i];
        temp += str_;
        if (isspace(str[i])) {
            // Skip forward until non-whitespace
            while (i < len && isspace(str[i])) {
                ++i;
            }
        }
        else {
            ++i;
        }
    }
    str = temp;
}

} // namespace kty
