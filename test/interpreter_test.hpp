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
    interpreter.execute(command);
    assertEqual(interpreter.get_device("answer").type, DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "7");
    alloc.stat();
}

test(interpreter_is_led)
{
    string command = "light IsLED(13, 25)";
    interpreter.execute(command);
    assertEqual(interpreter.get_device("light").type, DeviceType::LED);
    assertTrue(interpreter.get_device("light").info[0] == "13");
    assertTrue(interpreter.get_device("light").info[1] == "25");
    alloc.stat();
}

test(interpreter_if)
{
    vector<string> commands = {
        "answer IsNumber(42)",
        "If (answer = 42) (",
        "   answer IsNumber(answer + 10)",
        ")"
    };
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
    alloc.stat();
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
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("answer").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("answer").info[0] == "0");
    alloc.stat();
}

test(interpreter_fizz_buzz_1)
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
            "num MoveBy(1)",
        ")",
        "num IsNumber(1)",
        "fizzbuzz RunGroup(20)"
    };
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("num").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("num").info[0] == "21");
    alloc.stat();
}

test(interpreter_fizz_buzz_2)
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
            "num MoveBy(1)",
        ")",
        "num IsNumber(1)",
        "fizzbuzz RunGroup(20)"
    };
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertTrue(interpreter.get_device("num").type == DeviceType::NUM);
    assertTrue(interpreter.get_device("num").info[0] == "21");
    alloc.stat();
}
