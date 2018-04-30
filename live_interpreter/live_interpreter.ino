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
vector<Token> tokens;

Interface kty_interface;
Interpreter kty_interpreter;
Parser kty_parser;
Tokenizer kty_tokenizer;

void setup() {
    kty_interface.print_welcome();
    kty_interface.print_prompt();
}

void loop() {
    command = kty_interface.get_next_command();
    kty_interface.echo_command(command);
    tokens = kty_tokenizer.tokenize(command);
    tokens = kty_parser.parse(tokens);
    kty_interpreter.execute(tokens);
    kty_interface.print_prompt();
}
