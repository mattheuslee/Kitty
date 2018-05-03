#if !defined(ARDUINO)

#include <iostream>

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "ArduinoUnit.h"
#include "ArduinoUnitMock.h"

CppIOStream Serial;

#include <kitty.hpp>
#include <test/mock_arduino.hpp>
#include <test/mock_arduino_log.hpp>
MockArduinoLog Log;

#include <test/allocator_test.hpp>
#include <test/deque_test.hpp>
#include <test/string_utils_test.hpp>

#include <test/interpreter_test.hpp>
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>

void setup() {
}

void loop() {
    Test::run();
}


int main(void) {
    setup();
    Test::min_verbosity = TEST_VERBOSITY_ALL;

    while (Test::remaining() > 0) {
        std::cout << "Remaining: " << Test::remaining() << std::endl;
        loop();
    }

    return Test::getCurrentFailed();
}

#endif
