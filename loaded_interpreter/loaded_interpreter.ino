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

vector<string> commands = {
    "fizzbuzz IsGroup (",
        "If (num % 3 = 0 & num % 5 = 0) (",
        "    'FizzBuzz'",
        ")",
        "ElseIf (num % 3 = 0) (",
        "    'Fizz'",
        ")",
        "ElseIf (num % 5 = 0) (",
        "    'Buzz'",
        ")",
        "Else (",
            "num",
        ")",
        "num IsNumber(num + 1)",
    ")",
    "num IsNumber(1)",
    "fizzbuzz RunGroup()"
};
string command;

Interface interface;
Interpreter interpreter;

void setup() {
    interface.print_welcome();
    interface.print_prompt();

    for (int i = 0; i < commands.size(); ++i) {
        interface.echo_command(commands[i]);
        interpreter.execute(commands[i]);
        interface.print_prompt(interpreter.get_prompt_prefix());
    }
}

void loop() {
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    interface.print_prompt(interpreter.get_prompt_prefix());
}
