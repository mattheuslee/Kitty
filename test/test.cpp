#if !defined(ARDUINO)

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
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>

#include <kty/analyzer.hpp>
#include <kty/interpreter.hpp>
#include <kty/machine_state.hpp>
#include <kty/parser.hpp>
#include <kty/string_utils.hpp>
#include <kty/token.hpp>
#include <kty/tokenizer.hpp>
#include <kty/utils.hpp>

using namespace kty;

Allocator<>         alloc;
StringPool<>        stringPool;
GetAllocInit<>      getAllocInit(alloc);
GetStringPoolInit<> getStringPoolInit(stringPool);

Analyzer<>          analyzer;
Interpreter<>       interpreter;
MachineState<>      machineState;
Parser<>            parser;
Tokenizer<>         tokenizer;

#include <test/allocator_test.hpp>
#include <test/deque_test.hpp>
#include <test/deque_of_deque_test.hpp>
#include <test/string_test.hpp>
#include <test/stringpool_test.hpp>

#include <test/analyzer_test.hpp>
#include <test/interpreter_test.hpp>
#include <test/machine_state_test.hpp>
#include <test/parser_test.hpp>
#include <test/string_utils_test.hpp>
#include <test/token_test.hpp>
#include <test/tokenizer_test.hpp>
#include <test/utils_test.hpp>

int main(void) {
    Test::min_verbosity = TEST_VERBOSITY_TESTS_SUMMARY;
    //Test::min_verbosity = TEST_VERBOSITY_ALL;

    Log.to_log_verbose(false);
    Log.to_log_trace(false);
    Log.to_log_notice(false);
    Log.to_log_warning(false);
    Log.to_log_error(false);

    Test::exclude("*");
    Test::include("allocator*");
    Test::include("deque*");
    Test::include("string*");

    Test::include("analyzer*");
    Test::include("interpreter*");
    Test::include("machine_state*");
    Test::include("parser*");
    Test::include("token*");
    Test::include("tokenizer*");
    Test::include("utils*");

    Serial.println(F("Starting tests"));
    while (Test::remaining() > 0) {
        Test::run();
    }

    return Test::getCurrentFailed();
}

#endif
