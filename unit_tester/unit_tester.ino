#include <kitty.hpp>
#include <kitty/stl_impl.hpp>
#include <ArduinoUnit.h>
#include <string>

#include <kitty/tokenizer/tokenizer_test.hpp>

void setup() {
    Serial.begin(115200);
    while (!Serial);
}

void loop() {
    Test::run();
}
