#pragma once

#ifndef KITTY_VARIABLE_HPP_
#define KITTY_VARIABLE_HPP_

#define KITTY_VARIABLE_TYPE_INT_ 0
#define KITTY_VARIABLE_TYPE_DEVICE_SERVO_ 10
#define KITTY_VARIABLE_TYPE_DEVICE_LED_ 11
#define KITTY_VARIABLE_TYPE_DEVICE_SONAR_ 20

#include <StandardCplusplus.h>

#include <string>

#include <kitty_int.hpp>
#include <kitty_led.hpp>
#include <kitty_servo.hpp>
#include <kitty_sonar.hpp>

namespace kitty {

using std::string;

class kitty_variable {

public:
    kitty_variable() {}

    void set(kitty_variable const & kittyVar) {
        varType_ = kittyVar.varType_;
        intVar_ = kittyVar.intVar_;
    }

    void set(kitty_int const & intVar) {
        intVar_ = intVar;
        varType_ = KITTY_VARIABLE_TYPE_INT_;
    }

    void set(kitty_servo const & servo) {
        servo_ = servo;
        varType_ = KITTY_VARIABLE_TYPE_DEVICE_SERVO_;
    }

    void set(kitty_led const & led) {
        led_ = led;
        varType_ = KITTY_VARIABLE_TYPE_DEVICE_LED_;
    }

    void set(kitty_sonar const & sonar) {
        sonar_ = sonar;
        varType_ = KITTY_VARIABLE_TYPE_DEVICE_SONAR_;
    }

    string str() {
        switch (varType_) {
        case KITTY_VARIABLE_TYPE_INT_:
            return intVar_.str();
        case KITTY_VARIABLE_TYPE_DEVICE_SERVO_:
            return servo_.str();
        case KITTY_VARIABLE_TYPE_DEVICE_LED_:
            return led_.str();
        case KITTY_VARIABLE_TYPE_DEVICE_SONAR_:
            return sonar_.str();
        default:
            return "";
        }
    }

    bool is_sensor() {
        switch (varType_) {
        case KITTY_VARIABLE_TYPE_DEVICE_SONAR_:
            return true;
        default:
            return false;
        }
    }

    bool is_mover() {
        switch (varType_) {
        case KITTY_VARIABLE_TYPE_DEVICE_SERVO_:
            return true;
        case KITTY_VARIABLE_TYPE_DEVICE_LED_:
            return true;
        default:
            return false;
        }
    }

    bool is_value() {
        switch(varType_) {
        case KITTY_VARIABLE_TYPE_INT_:
            return true;
        default:
            return false;
        }
    }

    int get_value() {
        switch(varType_) {
        case KITTY_VARIABLE_TYPE_INT_:
            return intVar_.val();
        default:
            return 0;
        }
    }

    int sense(string const & type) {
        switch (varType_) {
        case KITTY_VARIABLE_TYPE_DEVICE_SONAR_:
            return sonar_.sense(type);
        default:
            return 0;
        }
    }

    void move_by(int const & value) {
        switch (varType_) {
        case KITTY_VARIABLE_TYPE_DEVICE_SERVO_:
            servo_.move_by(value);
            break;
        case KITTY_VARIABLE_TYPE_DEVICE_LED_:
            led_.move_by(value);
            break;
        }
    }

    void move_to(int const & value) {
        switch (varType_) {
        case KITTY_VARIABLE_TYPE_DEVICE_SERVO_:
            servo_.move_to(value);
            break;
        case KITTY_VARIABLE_TYPE_DEVICE_LED_:
            led_.move_to(value);
            break;
        }
    }

private:
    uint8_t varType_;
    kitty_int intVar_;
    kitty_servo servo_;
    kitty_led led_;
    kitty_sonar sonar_;

protected:

};

} // kitty

#endif //KITTY_VARIABLE_HPP_
