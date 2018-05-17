#pragma once

#include <kty/containers/string.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace kty;

test(interpreter_constructor)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_constructor starting.");
    Interpreter<> interpreter;

    Test::min_verbosity = prevTestVerbosity;
}

test(interpreter_reset)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_reset starting.");
    interpreter.reset();

    Test::min_verbosity = prevTestVerbosity;
}

test(interpreter_get_prompt_prefix)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_get_prompt_prefix starting.");
    interpreter.reset();
    PoolString<> command;

    assertEqual(interpreter.get_prompt_prefix().c_str(), "");
    command = "blink IsGroup (";
    interpreter.execute(command);
    assertEqual(interpreter.get_prompt_prefix().c_str(), "(blink) ");
    interpreter.reset();

    command = "If (1) (";
    interpreter.execute(command);
    assertEqual(interpreter.get_prompt_prefix().c_str(), "(IF) ");
    interpreter.reset();

    command = "Else (";
    interpreter.execute(command);
    assertEqual(interpreter.get_prompt_prefix().c_str(), "(ELSE) ");
    interpreter.reset();

    Test::min_verbosity = prevTestVerbosity;
}

test(interpreter_execute_single_command)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_execute_single_command starting.");
    interpreter.reset();
    PoolString<> name;
    PoolString<> command;

    // Empty command
    command = "";
    interpreter.execute(command);    

    name = "answer";
    command = "answer IsNumber(3 + (1 - 5)^2 / 4)";
    interpreter.execute(command);
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 7);

    command = "answer MoveBy(10)";
    interpreter.execute(command);
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 17);

    command = "answer MoveByFor(10, 100)";
    interpreter.execute(command);
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 17);

    command = "answer SetTo(100)";
    interpreter.execute(command);
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 100);

    command = "answer SetToFor(10, 100)";
    interpreter.execute(command);
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 100);

    command = "answer";
    interpreter.execute(command);

    name = "light";
    command = "light IsLED(13, 25)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 25);

    command = "light MoveBy(25)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 50);

    command = "light MoveByFor(25)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 50);

    command = "light MoveBy(51)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 100);

    command = "light MoveBy(-101)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 0);

    command = "light SetTo(0)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 0);

    command = "light SetTo(100)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 100);

    command = "light SetTo(-1)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 0);

    command = "light SetTo(101)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 100);

    command = "light SetToFor(0, 0)";
    interpreter.execute(command);
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 100);

    command = "light";
    interpreter.execute(command);

    command = "'this_string_should_print'";
    interpreter.execute(command);
    
    command = "\"this_string_should_print\"";
    interpreter.execute(command);

    command = "num IsNumber(10)";
    interpreter.execute(command);
    command = "Print(\"num is \", num, ' and num + 5 is ', (num + 5))";
    interpreter.execute(command);

    command = "Wait(1000)";
    interpreter.execute(command);    

    command = "non_existant_name";
    interpreter.execute(command);

    command = "non_existant_name MoveBy(0)";
    interpreter.execute(command);

    command = "non_existant_name MoveByFor(0, 0)";
    interpreter.execute(command);

    command = "non_existant_name SetTo(0)";
    interpreter.execute(command);

    command = "non_existant_name SetToFor(0, 0)";
    interpreter.execute(command);

    command = "non_existant_name RunGroup()";
    interpreter.execute(command);

    Test::min_verbosity = prevTestVerbosity;
}

test(interpreter_execute_if)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_execute_if starting.");
    interpreter.reset();
    PoolString<> name;
    Deque<PoolString<>> commands;

    commands.push_back(PoolString<>("answer IsNumber(42)"));
    commands.push_back(PoolString<>("If (answer = 42) ("));
    commands.push_back(PoolString<>("    answer MoveBy(10)"));
    commands.push_back(PoolString<>(")"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 52);

    commands.clear();
    commands.push_back(PoolString<>("answer IsNumber(42)"));
    commands.push_back(PoolString<>("If (~(answer = 0)) ("));
    commands.push_back(PoolString<>("    answer IsNumber(0)"));
    commands.push_back(PoolString<>(")"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 0);

    commands.clear();
    commands.push_back(PoolString<>("answer IsNumber(42)"));
    commands.push_back(PoolString<>("If (answer) ("));
    commands.push_back(PoolString<>("    Else ("));
    commands.push_back(PoolString<>("        answer IsNumber(0)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 42);

    Test::min_verbosity = prevTestVerbosity;
}

test(interpreter_execute_group)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_execute_group starting.");
    interpreter.reset();
    PoolString<> name;
    Deque<PoolString<>> commands;
    Deque<PoolString<>> groupCommands;
    Deque<PoolString<>> expectedGroupCommands;

    commands.clear();
    commands.push_back(PoolString<>("make_answer_zero IsGroup ("));
    commands.push_back(PoolString<>("    If (~(answer = 0)) ("));
    commands.push_back(PoolString<>("        answer IsNumber(0)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("answer IsNumber(42)"));
    commands.push_back(PoolString<>("make_answer_zero RunGroup()"));
    expectedGroupCommands.clear();
    expectedGroupCommands.push_back(PoolString<>(" If (~(answer = 0)) ("));
    expectedGroupCommands.push_back(PoolString<>(" answer IsNumber(0)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 0);
    name = "make_answer_zero";
    assertTrue(interpreter.group_exists(name));
    groupCommands = interpreter.get_group_commands(name);
    assertEqual(groupCommands.size(), expectedGroupCommands.size());
    for (int i = 0; i < groupCommands.size(); ++i) {
        assertEqual(groupCommands[i].c_str(), expectedGroupCommands[i].c_str(), "i = " << i);
    }
    interpreter.execute(PoolString<>("make_answer_zero"));

    commands.clear();
    commands.push_back(PoolString<>("make_answer_zero IsGroup ("));
    commands.push_back(PoolString<>("    If (answer > 0 | answer < 0) ("));
    commands.push_back(PoolString<>("        answer IsNumber(0)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("answer IsNumber(42)"));
    commands.push_back(PoolString<>("make_answer_zero RunGroup()"));
    expectedGroupCommands.clear();
    expectedGroupCommands.push_back(PoolString<>(" If (answer > 0 | answer < 0) ("));
    expectedGroupCommands.push_back(PoolString<>(" answer IsNumber(0)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 0);
    name = "make_answer_zero";
    assertTrue(interpreter.group_exists(name));
    groupCommands = interpreter.get_group_commands(name);
    assertEqual(groupCommands.size(), expectedGroupCommands.size());
    for (int i = 0; i < groupCommands.size(); ++i) {
        assertEqual(groupCommands[i].c_str(), expectedGroupCommands[i].c_str(), "i = " << i);
    }
    interpreter.execute(PoolString<>("make_answer_zero"));

    commands.clear();
    commands.push_back(PoolString<>("flip_answer IsGroup ("));
    commands.push_back(PoolString<>("    If (answer > 0) ("));
    commands.push_back(PoolString<>("        answer IsNumber(-1 * answer)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    Else ("));
    commands.push_back(PoolString<>("        answer IsNumber(-answer)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("answer IsNumber(-42)"));
    commands.push_back(PoolString<>("flip_answer RunGroup()"));
    expectedGroupCommands.clear();
    expectedGroupCommands.push_back(PoolString<>(" If (answer > 0) ("));
    expectedGroupCommands.push_back(PoolString<>(" answer IsNumber(-1 * answer)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    expectedGroupCommands.push_back(PoolString<>(" Else ("));
    expectedGroupCommands.push_back(PoolString<>(" answer IsNumber(-answer)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 42);
    name = "flip_answer";
    assertTrue(interpreter.group_exists(name));
    groupCommands = interpreter.get_group_commands(name);
    assertEqual(groupCommands.size(), expectedGroupCommands.size());
    for (int i = 0; i < groupCommands.size(); ++i) {
        assertEqual(groupCommands[i].c_str(), expectedGroupCommands[i].c_str(), "i = " << i);
    }

    commands.clear();
    commands.push_back(PoolString<>("flip_answer IsGroup ("));
    commands.push_back(PoolString<>("    If (answer > 0) ("));
    commands.push_back(PoolString<>("        answer IsNumber(-1 * answer)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    Else ("));
    commands.push_back(PoolString<>("        answer IsNumber(-answer)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("answer IsNumber(42)"));
    commands.push_back(PoolString<>("flip_answer RunGroup()"));
    expectedGroupCommands.clear();
    expectedGroupCommands.push_back(PoolString<>(" If (answer > 0) ("));
    expectedGroupCommands.push_back(PoolString<>(" answer IsNumber(-1 * answer)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    expectedGroupCommands.push_back(PoolString<>(" Else ("));
    expectedGroupCommands.push_back(PoolString<>(" answer IsNumber(-answer)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "answer";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), -42);
    name = "flip_answer";
    assertTrue(interpreter.group_exists(name));
    groupCommands = interpreter.get_group_commands(name);
    assertEqual(groupCommands.size(), expectedGroupCommands.size());
    for (int i = 0; i < groupCommands.size(); ++i) {
        assertEqual(groupCommands[i].c_str(), expectedGroupCommands[i].c_str(), "i = " << i);
    }

    commands.clear();
    commands.push_back(PoolString<>("bright_threshold IsGroup ("));
    commands.push_back(PoolString<>("    If (light >= 50) ("));
    commands.push_back(PoolString<>("        light MoveBy(50)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    If (light <= 49) ("));
    commands.push_back(PoolString<>("        light MoveBy(-50)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("light IsLED(13, 50)"));
    commands.push_back(PoolString<>("bright_threshold RunGroup()"));
    expectedGroupCommands.clear();
    expectedGroupCommands.push_back(PoolString<>(" If (light >= 50) ("));
    expectedGroupCommands.push_back(PoolString<>(" light MoveBy(50)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    expectedGroupCommands.push_back(PoolString<>(" If (light <= 49) ("));
    expectedGroupCommands.push_back(PoolString<>(" light MoveBy(-50)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "light";
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 100);
    name = "bright_threshold";
    assertTrue(interpreter.group_exists(name));
    groupCommands = interpreter.get_group_commands(name);
    assertEqual(groupCommands.size(), expectedGroupCommands.size());
    for (int i = 0; i < groupCommands.size(); ++i) {
        assertEqual(groupCommands[i].c_str(), expectedGroupCommands[i].c_str(), "i = " << i);
    }

    commands.clear();
    commands.push_back(PoolString<>("bright_threshold IsGroup ("));
    commands.push_back(PoolString<>("    If (light >= 50) ("));
    commands.push_back(PoolString<>("        light MoveBy(50)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    If (light <= 49) ("));
    commands.push_back(PoolString<>("        light MoveBy(-50)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("light IsLED(13, 49)"));
    commands.push_back(PoolString<>("bright_threshold RunGroup()"));
    expectedGroupCommands.clear();
    expectedGroupCommands.push_back(PoolString<>(" If (light >= 50) ("));
    expectedGroupCommands.push_back(PoolString<>(" light MoveBy(50)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    expectedGroupCommands.push_back(PoolString<>(" If (light <= 49) ("));
    expectedGroupCommands.push_back(PoolString<>(" light MoveBy(-50)"));
    expectedGroupCommands.push_back(PoolString<>(" )"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "light";
    assertTrue(interpreter.device_exists(name));
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertEqual(interpreter.get_device_info(name, 0), -1);
    assertEqual(interpreter.get_device_info(name, 1), 13);
    assertEqual(interpreter.get_device_info(name, 2), 0);
    name = "bright_threshold";
    assertTrue(interpreter.group_exists(name));
    groupCommands = interpreter.get_group_commands(name);
    assertEqual(groupCommands.size(), expectedGroupCommands.size());
    for (int i = 0; i < groupCommands.size(); ++i) {
        assertEqual(groupCommands[i].c_str(), expectedGroupCommands[i].c_str(), "i = " << i);
    }

    Test::min_verbosity = prevTestVerbosity;
}

test(interpreter_fizz_buzz)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test interpreter_fizz_buzz starting.");
    interpreter.reset();
    PoolString<> name;
    Deque<PoolString<>> commands;

    commands.clear();
    commands.push_back(PoolString<>("fizzbuzz IsGroup ("));
    commands.push_back(PoolString<>("    If (num % 3 = 0 & num % 5 = 0) ("));
    commands.push_back(PoolString<>("        fizzbuzz_num MoveBy(1)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    If (num % 3 = 0 & ~(num % 5 = 0)) ("));
    commands.push_back(PoolString<>("        fizz_num MoveBy(1)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    If (~(num % 3 = 0) & num % 5 = 0) ("));
    commands.push_back(PoolString<>("        buzz_num MoveBy(1)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    If (~(num % 3 = 0) & ~(num % 5 = 0)) ("));
    commands.push_back(PoolString<>("        num_num MoveBy(1)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    num MoveBy(1)"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("num IsNumber(1)")); 
    commands.push_back(PoolString<>("num_num IsNumber(0)")); 
    commands.push_back(PoolString<>("fizz_num IsNumber(0)")); 
    commands.push_back(PoolString<>("buzz_num IsNumber(0)")); 
    commands.push_back(PoolString<>("fizzbuzz_num IsNumber(0)")); 
    commands.push_back(PoolString<>("fizzbuzz RunGroup(20)"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 21);
    name = "num_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 11);
    name = "fizz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 5);
    name = "buzz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 3);
    name = "fizzbuzz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 1);

    commands.clear();
    commands.push_back(PoolString<>("fizzbuzz IsGroup ("));
    commands.push_back(PoolString<>("    If (num % 3 = 0 & num % 5 = 0) ("));
    commands.push_back(PoolString<>("        fizzbuzz_num MoveBy(1)"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    Else ("));
    commands.push_back(PoolString<>("        If (num % 3 = 0) ("));
    commands.push_back(PoolString<>("            fizz_num MoveBy(1)"));
    commands.push_back(PoolString<>("        )"));
    commands.push_back(PoolString<>("        Else ("));
    commands.push_back(PoolString<>("            If (num % 5 = 0) ("));
    commands.push_back(PoolString<>("                buzz_num MoveBy(1)"));
    commands.push_back(PoolString<>("            )"));
    commands.push_back(PoolString<>("            Else ("));
    commands.push_back(PoolString<>("                num_num MoveBy(1)"));
    commands.push_back(PoolString<>("            )"));
    commands.push_back(PoolString<>("        )"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    num MoveBy(1)"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("num IsNumber(1)"));
    commands.push_back(PoolString<>("num_num IsNumber(0)"));
    commands.push_back(PoolString<>("fizz_num IsNumber(0)"));
    commands.push_back(PoolString<>("buzz_num IsNumber(0)"));
    commands.push_back(PoolString<>("fizzbuzz_num IsNumber(0)"));
    commands.push_back(PoolString<>("fizzbuzz RunGroup(20)"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 21);
    name = "num_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 11);
    name = "fizz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 5);
    name = "buzz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 3);
    name = "fizzbuzz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 1);

    commands.clear();
    commands.push_back(PoolString<>("fizzbuzz IsGroup ("));
    commands.push_back(PoolString<>("    If (num % 3 = 0) ("));
    commands.push_back(PoolString<>("        If (num % 5 = 0) ("));
    commands.push_back(PoolString<>("            fizzbuzz_num MoveBy(1)"));
    commands.push_back(PoolString<>("        )"));
    commands.push_back(PoolString<>("        Else ("));
    commands.push_back(PoolString<>("            fizz_num MoveBy(1)"));
    commands.push_back(PoolString<>("        )"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    Else ("));
    commands.push_back(PoolString<>("        If (num % 5 = 0) ("));
    commands.push_back(PoolString<>("            buzz_num MoveBy(1)"));
    commands.push_back(PoolString<>("        )"));
    commands.push_back(PoolString<>("        Else ("));
    commands.push_back(PoolString<>("            num_num MoveBy(1)"));
    commands.push_back(PoolString<>("        )"));
    commands.push_back(PoolString<>("    )"));
    commands.push_back(PoolString<>("    num MoveBy(1)"));
    commands.push_back(PoolString<>(")"));
    commands.push_back(PoolString<>("num IsNumber(1)"));
    commands.push_back(PoolString<>("num_num IsNumber(0)"));
    commands.push_back(PoolString<>("fizz_num IsNumber(0)"));
    commands.push_back(PoolString<>("buzz_num IsNumber(0)"));
    commands.push_back(PoolString<>("fizzbuzz_num IsNumber(0)"));
    commands.push_back(PoolString<>("fizzbuzz RunGroup(20)"));
    for (auto & command : commands) {
        interpreter.execute(command);
    }
    name = "num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 21);
    name = "num_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 11);
    name = "fizz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 5);
    name = "buzz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 3);
    name = "fizzbuzz_num";
    assertTrue(interpreter.number_exists(name));
    assertEqual(interpreter.get_number_value(name), 1);

    Test::min_verbosity = prevTestVerbosity;
}
