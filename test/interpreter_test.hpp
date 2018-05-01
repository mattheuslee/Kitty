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
