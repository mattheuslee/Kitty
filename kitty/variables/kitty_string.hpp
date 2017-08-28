#pragma once

#ifndef KITTY_STRING_HPP_
#define KITTY_STRING_HPP_

#include <kitty/kitty_stl_impl.hpp>

#include <string>

namespace kitty {

using std::string;

class kitty_string {

public:
    kitty_string() {}

    explicit kitty_string(string const & s)
            : str_(s) {}

    string str() const {
        return "\"" + str_ + "\"";
    }

    void print() const {
        Serial.println(str_.c_str());
    }

private:
    string str_;

protected:

};

} // kitty

#endif //KITTY_STRING_HPP_
