#include <StandardCplusplus.h>

#include <string>

#include <kitty_interpreter.hpp>

using namespace std;
using namespace kitty;

string input;

kitty_interpreter interpreter;

void setup() {
    Serial.begin(115200);
    Serial.println(F("Kitty 0.0.1"));
    Serial.print(F(">>>"));
}

void loop() {
    while (1) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                break;
            }
            input += c;
        }
    }
    if (!input.empty()) {
        Serial.println(input.c_str()); // echo back to the user
        interpreter.parse_and_execute_input(input);
        input = "";
        Serial.print(F(">>>"));
    }
}
