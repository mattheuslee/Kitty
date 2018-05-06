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
MockArduinoLog Log(false, false);

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/string_utils.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace kty;

Allocator<200, sizeof(int) * 8> alloc;
StringPool<50, 40> stringPool;

Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);
Parser parser;
Tokenizer tokenizer;

#include <test/allocator_test.hpp>
#include <test/deque_test.hpp>
#include <test/string_test.hpp>
#include <test/string_utils_test.hpp>
#include <test/interpreter_test.hpp>
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>

int main(void) {
    //Test::min_verbosity = TEST_VERBOSITY_ALL;

    while (Test::remaining() > 0) {
        Test::run();
    }

    return Test::getCurrentFailed();
}

#endif
