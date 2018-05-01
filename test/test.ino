#include "kitty.hpp"
#include "ArduinoUnit.h"

#include "test/interpreter_test.hpp"
#include <test/parser_test.hpp>
#include <test/tokenizer_test.hpp>
#include <test/string_utils_test.hpp>

void setup() {
    Serial.begin(115200);
    while (!Serial);
}

void loop() {
    Test::run();
}
