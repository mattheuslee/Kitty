#if !defined(ARDUINO)

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "ArduinoUnit.h"
#include "ArduinoUnitMock.h"

#include "mock_arduino_pins.hpp"

void setup();
void loop();

CppIOStream Serial;

int main(void) {
    setup();

    while (Test::remaining() > 0) {
        loop();
    }

    return 0;
}

#include "test.ino"

#endif
