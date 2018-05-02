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

Interface interface;
Interpreter interpreter;

void setup() {
    interface.print_welcome();
    interface.print_prompt();
}

void loop() {
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    interface.print_prompt(interpreter.get_prompt_prefix());
}
