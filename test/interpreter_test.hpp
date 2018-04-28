#pragma once

#include <kitty/stl_impl.hpp>
#include <string>
#include <vector>

#include <kitty/interpreter/interpreter.hpp>
#include <kitty/parser/parser.hpp>
#include <kitty/tokenizer/tokenizer.hpp>

using namespace std;
using namespace kitty;
using namespace kitty::interpreter;
using namespace kitty::tokenizer;
using namespace kitty::parser;

test(interpreter_is_number)
{
    string command = "answer IsNumber(42)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();

    Interpreter interpreter;
    interpreter.execute(tokens);

    assertTrue(interpreter.get_device("answer").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "42");
}

test(interpreter_is_led)
{
    string command = "light IsLED(13, 25)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();

    Interpreter interpreter;
    interpreter.execute(tokens);

    assertTrue(interpreter.get_device("light").type == DeviceType::LED);
    assertTrue(interpreter.get_device("light").info[0] == "13");
    assertTrue(interpreter.get_device("light").info[1] == "25");
}
