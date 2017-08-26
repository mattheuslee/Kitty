#pragma once

#ifndef KITTY_LED_HPP_
#define KITTY_LED_HPP_

#include <StandardCplusplus.h>

#include <sstream>
#include <string>

namespace kitty {

using std::ostringstream;
using std::string;

class kitty_led {

public:
    kitty_led()
            : isActive_(false), pin_(0), level_(0) {}

    explicit kitty_led(int const & pin)
            : isActive_(true), pin_(pin), level_(0) {
        isActive_ = true;
        pinMode(pin_, OUTPUT);
        analogWrite(pin_, (int)((level_ / 100.0) * 255));
    }

    string str() const {
        ostringstream oss;
        oss << " led using pin " << pin_ << " at level " << level_ << "%";
        return oss.str();
    }

    void print() const {
        Serial.println(str().c_str());
    }

    int val() const {
        return level_;
    }

    void move_by(int const & value) {
        if (isActive_) {
            level_ += value;
            level_ = constrain(level_, 0, 100);
            analogWrite(pin_, (int)((level_ / 100.0) * 255));
        }
    }

    void move_to(int const & value) {
        if (isActive_) {
            level_ = value;
            level_ = constrain(level_, 0, 100);
            analogWrite(pin_, (int)((level_ / 100.0) * 255));
        }
    }

    void move_by_for(int const & value, int const & duration) {
        if (isActive_) {
            auto origLevel = level_;
            move_by(value);
            delay(duration);
            move_to(origLevel);
        }
    }

    void move_to_for(int const & value, int const & duration) {
        if (isActive_) {
            auto origLevel = level_;
            move_to(value);
            delay(duration);
            move_to(origLevel);
        }
    }

private:
    bool isActive_;
    int pin_;
    int level_;

protected:

};

} // kitty

#endif //KITTY_LED_HPP_
