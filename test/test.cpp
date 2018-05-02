#if !defined(ARDUINO)

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "ArduinoUnit.h"
#include "ArduinoUnitMock.h"
#include <ArduinoLog.h>

CppIOStream Serial;

#include <kitty.hpp>
#include <test/mock_arduino_pins.hpp>
#include <test/interpreter_test.hpp>
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>
#include <test/string_utils_test.hpp>

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Log.begin(LOG_LEVEL_SILENT, &Serial);
}

void loop() {
    Test::run();
}


int main(void) {
    setup();

    while (Test::remaining() > 0) {
        loop();
    }

    return Test::getCurrentFailed();
}

#endif
