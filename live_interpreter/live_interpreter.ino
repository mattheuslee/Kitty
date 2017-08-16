#include <StandardCplusplus.h>

#include <string>

#include <kitty_interpreter.hpp>

using namespace std;
using namespace kitty;

string input;

kitty_interpreter interpreter;

void setup() {
    interpreter.print_welcome_screen();
    interpreter.print_prompt();
}

void loop() {
    input = get_input_line();
    echo_input_line(input);
    interpreter.execute(input);
    input = "";
    interpreter.print_prompt();
}

string get_input_line() {
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
    if (line[line.size() - 1] == ' ') {
        line.erase(line.size() - 1);
    }
    return line;
}

void echo_input_line(string const & input) {
    Serial.println(input.c_str());
}
