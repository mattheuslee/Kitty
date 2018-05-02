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

    commands = {
        "answer IsNumber(42)",
        "If (~(answer = 0)) (",
        "   answer IsNumber(0)",
        ")"
    };
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("answer").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "0");
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

test(interpreter_fizz_buzz)
{
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
        "fizzbuzz RunGroup(5)"
    };
    Interpreter interpreter;
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("num").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("num").info[0] == "6");
}

test(interpreter_fizz_buzz_alternative_1)
{
    vector<string> commands = {
        "fizzbuzz IsGroup (",
            "If (num % 3 = 0 & num % 5 = 0) (",
            "    'FizzBuzz'",
            ")",
            "If (num % 3 = 0 & ~(num % 5 = 0)) (",
            "    'Fizz'",
            ")",
            "If (~(num % 3 = 0) & num % 5 = 0) (",
            "    'Buzz'",
            ")",
            "If (~(num % 3 = 0) & ~(num % 5 = 0)) (",
                "num",
            ")",
            "num IsNumber(num + 1)",
        ")",
        "num IsNumber(1)",
        "fizzbuzz RunGroup(5)"
    };
    Interpreter interpreter;
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("num").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("num").info[0] == "6");
}

test(interpreter_fizz_buzz_alternative_2)
{
    vector<string> commands = {
        "fizzbuzz IsGroup (",
            "If (num % 3 = 0 & num % 5 = 0) (",
                "'FizzBuzz'",
            ")",
            "Else (",
                "If (num % 3 = 0) (",
                    "'Fizz'",
                ")",
                "Else (",
                    "If (num % 5 = 0) (",
                        "'Buzz'",
                    ")",
                    "Else (",
                        "num",
                    ")",
                ")",
            ")",
            "num IsNumber(num + 1)",
        ")",
        "num IsNumber(1)",
        "fizzbuzz RunGroup(5)"
    };
    Interpreter interpreter;
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("num").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("num").info[0] == "6");
}
