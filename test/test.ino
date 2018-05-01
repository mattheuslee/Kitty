#include <kitty.hpp>
#include <kty/stl_impl.hpp>
#include <ArduinoUnit.h>
#include <string>

#include <test/interpreter_test.hpp>
//#include <test/parser_test.hpp>
//#include <test/tokenizer_test.hpp>
//#include <test/string_utils_test.hpp>

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Test::exclude("*");
    Test::include("interpreter_is_number*");
    //Test::include("parser*");
    //Test::include("tokenizer*");
    //Test::include("string_utils*");
}

void loop() {
    Test::run();
}
