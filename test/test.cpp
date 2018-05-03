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

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/string_utils.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace kty;

// 120 seems to be safe enough to run all the tests
Allocator<Deque<int>::Node> dequeIntAlloc = Deque<int>::create_allocator(120);
Interpreter interpreter(dequeIntAlloc);
Parser parser;
Tokenizer tokenizer;

#include <test/allocator_test.hpp>
#include <test/deque_test.hpp>
#include <test/string_utils_test.hpp>
#include <test/interpreter_test.hpp>
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>

int main(void) {
    Test::min_verbosity = TEST_VERBOSITY_ALL;

    while (Test::remaining() > 0) {
        Test::run();
    }

    return Test::getCurrentFailed();
}

#endif
