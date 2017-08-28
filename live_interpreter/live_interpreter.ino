#include <kitty.hpp>
#include <kitty/kitty_stl_impl.hpp>

#include <string>

#include <kitty/kitty_interpreter.hpp>

using namespace std;
using namespace kitty;

string input;

kitty_interpreter interpreter;

void setup() {
    interpreter.print_welcome_screen();
    interpreter.print_prompt();
}

void loop() {
    input = kitty_utility::get_line();
    echo_input_line(input);
    interpreter.execute(input);
    input = "";
    interpreter.print_prompt();
}

void echo_input_line(string const & input) {
    Serial.println(input.c_str());
}
