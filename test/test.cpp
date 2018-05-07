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
MockArduinoLog Log(false,  // Log trace
                   false,  // Log notice
                   false); // Log warning

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/sizes.hpp>
#include <kty/string_utils.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace kty;

Allocator<200, Sizes::alloc_size> alloc;
StringPool<50, Sizes::string_length> stringPool;

Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);
Parser parser;
Tokenizer tokenizer;

#include <test/allocator_test.hpp>
#include <test/deque_test.hpp>
#include <test/deque_of_deque_test.hpp>
#include <test/string_test.hpp>
#include <test/string_utils_test.hpp>
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>
#include <test/interpreter_test.hpp>

int main(void) {
    Test::min_verbosity = TEST_VERBOSITY_TESTS_SUMMARY;
    //Test::min_verbosity = TEST_VERBOSITY_ALL;

    Test::exclude("*");
    Test::include("allocator*");
    Test::include("deque*");
    Test::include("deque_of_deque*");
    Test::include("string_stringpool*");
    Test::include("string_poolstring*");
    Test::include("string_deque_of_poolstring*");
    Test::include("string_utils_str_to_int*");
    Test::include("string_utils_int_to_str*");
    Test::include("string_utils_remove_str_whitespace*");
    Test::include("parser*");
    Test::include("tokenizer*");
    Test::include("interpreter*");

    while (Test::remaining() > 0) {
        Test::run();
    }

    return Test::getCurrentFailed();
}

#endif
