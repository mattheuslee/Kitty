#pragma once

#include <kitty/stl_impl.hpp>
#include <sstream>
#include <string>

namespace kitty {

namespace utils {

int str_to_int(std::string const & str) {
    if (str.empty()) {
        return 0;
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

std::string get_line_from_serial() {
    std::string line;
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') { // Finished reading one complete line of input
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

std::string remove_str_whitespace(std::string const & str) {
    std::string result = str;
    while (result.find(' ') != std::string::npos) {
        result.erase(result.find(' '), 1);
    }
    return result;
}

} // namespace utils

} // namespace kitty
