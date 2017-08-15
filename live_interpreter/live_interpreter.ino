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
    input = get_line_();
    if (!input.empty()) {
        Serial.println(input.c_str()); // echo back to the user
        interpreter.execute(input);
        input = "";
        Serial.print(F(">>>"));
    }
}

string get_line_() {
    string line;
    while (1) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                break;
            }
            line += c;
        }
    }
    return line;
}
