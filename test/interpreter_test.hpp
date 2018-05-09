#pragma once

#include <kty/stl_impl.hpp>

#include <kty/containers/string.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace std;
using namespace kty;

test(interpreter_is_number)
{
    Log.notice(F("Test interpreter_is_number starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "answer");
    PoolString<decltype(stringPool)> command(stringPool, "answer IsNumber(3 + (1 - 5)^2 / 4)");
    interpreter.execute(command);
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 7);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}

test(interpreter_is_led)
{
    Log.notice(F("Test interpreter_is_led starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "light");
    PoolString<decltype(stringPool)> command(stringPool, "light IsLED(13, 25)");
    interpreter.execute(command);
    assertEqual(interpreter.get_device_type(name), DeviceType::LED);
    assertTrue(interpreter.get_device_info(name, 1) == 13);
    assertTrue(interpreter.get_device_info(name, 2) == 25);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}

test(interpreter_if)
{
    Log.notice(F("Test interpreter_if starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "answer");
    Deque<PoolString<decltype(stringPool)>, decltype(alloc)> commands(alloc);
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "answer IsNumber(42)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "If (answer = 42) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    answer MoveBy(10)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, ")"));

    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 52);

    commands.clear();
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "answer IsNumber(42)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "If (~(answer = 0)) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    answer IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, ")"));

    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 0);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}

test(interpreter_if_group)
{
    Log.notice(F("Test interpreter_if_group starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "answer");
    Deque<PoolString<decltype(stringPool)>, decltype(alloc)> commands(alloc);
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "make_answer_zero IsGroup ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (answer > 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        answer IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, ")"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "answer IsNumber(42)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "make_answer_zero RunGroup()"));

    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 0);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}

test(interpreter_fizz_buzz_1)
{
    Log.notice(F("Test interpreter_fizz_buzz_1 starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "num");
    Deque<PoolString<decltype(stringPool)>, decltype(alloc)> commands(alloc);
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz IsGroup ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (num % 3 = 0 & num % 5 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        'FizzBuzz'"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (num % 3 = 0 & ~(num % 5 = 0)) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        'Fizz'"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (~(num % 3 = 0) & num % 5 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        \"Buzz\""));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (~(num % 3 = 0) & ~(num % 5 = 0)) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        num"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, ")"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "num IsNumber(1)")); 
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz RunGroup(20)"));
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 21);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}


test(interpreter_fizz_buzz_2)
{
    Log.notice(F("Test interpreter_fizz_buzz_2 starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "num");
    Deque<PoolString<decltype(stringPool)>, decltype(alloc)> commands(alloc);
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz IsGroup ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (num % 3 = 0 & num % 5 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        fizzbuzz_num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    Else ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        If (num % 3 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            fizz_num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        Else ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            If (num % 5 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "                buzz_num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            Else ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "                num"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, ")"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "num IsNumber(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizz_num IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "buzz_num IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz_num IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz RunGroup(20)"));
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 21);
    name = "fizz_num";
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 5);
    name = "buzz_num";
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 3);
    name = "fizzbuzz_num";
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 1);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}

test(interpreter_fizz_buzz_3)
{
    Log.notice(F("Test interpreter_fizz_buzz_3 starting\n"));
    interpreter.reset();
    PoolString<decltype(stringPool)> name(stringPool, "num");
    Deque<PoolString<decltype(stringPool)>, decltype(alloc)> commands(alloc);
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz IsGroup ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    If (num % 3 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        If (num % 5 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            fizzbuzz_num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        Else ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            fizz_num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    Else ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        If (num % 5 = 0) ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            buzz_num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        Else ("));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "            num"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "        )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    )"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "    num MoveBy(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, ")"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "num IsNumber(1)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizz_num IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "buzz_num IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz_num IsNumber(0)"));
    commands.push_back(PoolString<decltype(stringPool)>(stringPool, "fizzbuzz RunGroup(20)"));
    for (int i = 0; i < commands.size(); ++i) {
        interpreter.execute(commands[i]);
    }
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 21);
    name = "fizz_num";
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 5);
    name = "buzz_num";
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 3);
    name = "fizzbuzz_num";
    assertEqual(interpreter.get_device_type(name), DeviceType::NUM);
    assertEqual(interpreter.get_device_info(name, 2), 1);
    alloc.stat();
    stringPool.stat();
    alloc.reset_stat();
    stringPool.reset_stat();
}
