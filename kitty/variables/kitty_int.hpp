#pragma once

#ifndef KITTY_INT_HPP_
#define KITTY_INT_HPP_

#include <kitty/kitty_stl_impl.hpp>

#include <string>

namespace kitty {

using std::string;

class kitty_int {

public:
    kitty_int()
            :val_(0) {}

    explicit kitty_int(int const & i)
            : val_(i) {}

    string str() const {
        String val(val_);
        return string(val.c_str());
    }

    void print() const {
        Serial.println(str().c_str());
    }

    int val() const {
        return val_;
    }

private:
    int val_;

protected:

};

} // kitty

#endif //KITTY_INT_HPP_
