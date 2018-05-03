#pragma once

#if !defined(ARDUINO)

#define OUTPUT 0
#define INPUT 1
#define HIGH 1
#define LOW 0

#define F(string) 1

void pinMode(int const & pin, int const & mode) {

}

void digitalWrite(int const & pin, int const & value) {

}

void analogWrite(int const & pin, int const & value) {

}

void delay(int millis) {
    
}

#endif
