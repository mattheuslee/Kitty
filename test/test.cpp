#if !defined(ARDUINO)

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "ArduinoUnit.h"
#include "ArduinoUnitMock.h"

#include <kitty.hpp>
#include <test/mock_arduino_pins.hpp>
#include <test/interpreter_test.hpp>
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>
#include <test/string_utils_test.hpp>

CppIOStream Serial;

void setup() {
    Serial.begin(115200);
    while (!Serial);
}

void loop() {
    Test::run();
}


int main(void) {
    setup();

    while (Test::remaining() > 0) {
        loop();
    }

    return 0;
}

#endif
