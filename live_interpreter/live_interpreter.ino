#include <kitty.hpp>
#include <kitty/stl_impl.hpp>
#include <string>
#include <vector>

#include <kitty/interface/interface.hpp>
#include <kitty/interpreter/interpreter.hpp>
#include <kitty/parser/parser.hpp>
#include <kitty/tokenizer/tokenizer.hpp>

using namespace std;
using namespace kitty;
using namespace kitty::interface;
using namespace kitty::interpreter;
using namespace kitty::parser;
using namespace kitty::tokenizer;

string command;
vector<Token> tokens;

Interface kitty_interface;
Interpreter kitty_interpreter;
Parser kitty_parser;
Tokenizer kitty_tokenizer;

void setup() {
    kitty_interface.print_welcome();
    kitty_interface.print_prompt();
}

void loop() {
    command = kitty_interface.get_next_command();
    kitty_interface.echo_command(command);
    tokens = kitty_tokenizer.tokenize(command);
    tokens = kitty_parser.parse(tokens);
    kitty_interpreter.execute(tokens);
    kitty_interface.print_prompt();
}
