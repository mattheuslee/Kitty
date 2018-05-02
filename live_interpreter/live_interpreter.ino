#include <kitty.hpp>
#include <kty/stl_impl.hpp>
#include <string>
#include <vector>
#include <ArduinoLog.h>

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
    interface.begin_logging(LOG_LEVEL_TRACE);
    interface.print_prompt();
}

void loop() {
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    interface.print_prompt(interpreter.get_prompt_prefix());
}
