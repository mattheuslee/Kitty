#include <kitty.hpp>
#include <kty/stl_impl.hpp>
#include <string>
#include <vector>

#include <kty/interface.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace std;
using namespace kty;

string command;

Interface kty_interface;
Interpreter kty_interpreter;

void setup() {
    kty_interface.print_welcome();
    kty_interface.print_prompt();
}

void loop() {
    command = kty_interface.get_next_command();
    kty_interface.echo_command(command);
    kty_interpreter.execute(command);
    kty_interface.print_prompt(kty_interpreter.get_prompt_prefix());
}
