#pragma once

#include <kty/stl_impl.hpp>
#include <string>
#include <vector>

#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace std;
using namespace kty;

test(interpreter_is_number)
{
    string command = "answer IsNumber(3 + (1 - 5)^2 / 4)";
    Interpreter interpreter;
    interpreter.execute(command);
    assertTrue(interpreter.get_device("answer").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "7");
}

test(interpreter_is_led)
{
    string command = "light IsLED(13, 25)";
    Interpreter interpreter;
    interpreter.execute(command);
    assertTrue(interpreter.get_device("light").type == DeviceType::LED);
    assertTrue(interpreter.get_device("light").info[0] == "13");
    assertTrue(interpreter.get_device("light").info[1] == "25");
}

test(interpreter_if)
{
    vector<string> commands = {
        "answer IsNumber(42)",
        "If (answer = 42) (",
        "   answer IsNumber(answer + 10)",
        ")"
    };
    Interpreter interpreter;
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("answer").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "52");
}

test(interpreter_if_group)
{
    vector<string> commands = {
        "make_answer_zero IsGroup (",
        "   If (answer > 0) (",
        "       answer IsNumber(0)",
        "   )",
        ")",
        "answer IsNumber(42)",
        "make_answer_zero RunGroup()"
    };
    Interpreter interpreter;
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("answer").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "0");
}
