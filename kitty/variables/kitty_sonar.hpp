#pragma once

#ifndef KITTY_SONAR_HPP_
#define KITTY_SONAR_HPP_

#include <StandardCplusplus.h>

#include <sstream>
#include <string>

#include <NewPing.h>

namespace kitty {

using std::ostringstream;
using std::string;

class kitty_sonar {

public:
    kitty_sonar()
            : isActive_(false), triggerPin_(0), echoPin_(0) {}

    kitty_sonar(int const & triggerPin, int const & echoPin)
            : isActive_(true), triggerPin_(triggerPin), echoPin_(echoPin) {
        sonar_ = NewPing(triggerPin_, echoPin_);
    }

    string str() const {
        ostringstream oss;
        oss << " sonar using trigger pin " << triggerPin_ << " and echo pin " << echoPin_;
        return oss.str();
    }

    void print() const {
        Serial.println(str().c_str());
    }

    int val() const {
        return sense("cm");
    }

    int sense(string const & type) const {
        if (!isActive_) {
            return 0;
        }
        if (type == "" || type == "cm") {
            return sonar_.ping_cm();
        } else if (type == "in") {
            return sonar_.ping_in();
        } else {
            return 0;
        }
    }

private:
    bool isActive_;
    int triggerPin_;
    int echoPin_;
    NewPing sonar_ = NewPing(0, 0);

protected:

};

} // kitty

#endif //KITTY_SONAR_HPP_
