#pragma once

#ifndef KITTY_SERVO_HPP_
#define KITTY_SERVO_HPP_

#include <Servo.h>

#include <StandardCplusplus.h>

#include <sstream>
#include <string>

namespace kitty {

using std::ostringstream;
using std::string;

class kitty_servo {

public:
    kitty_servo()
            : isActive_(false), pin_(0), value_(90) {}

    explicit kitty_servo(int const & pin)
            : isActive_(true), pin_(pin), value_(90) {
        servo_.attach(pin_);
        servo_.write(value_);
    }

    string str() const {
        ostringstream oss;
        oss << " servo using pin " << pin_ << " at value " << value_ << " degrees";
        return oss.str();
    }

    void move_by(int const & value) {
        if (isActive_) {
            value_ += value;
            value_ = constrain(value_, 0, 180);
            servo_.write(value_);
        }
    }

    void move_to(int const & value) {
        if (isActive_) {
            value_ = value;
            value_ = constrain(value_, 0, 180);
            servo_.write(value_);
        }
    }

private:
    bool isActive_;
    int pin_;
    int value_;
    Servo servo_;

protected:

};

} // kitty

#endif //KITTY_SERVO_HPP_
