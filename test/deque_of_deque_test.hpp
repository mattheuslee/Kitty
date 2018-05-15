#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>

using namespace kty;

test(deque_of_deque_dequedequepoolstring_constructor)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_of_deque_dequedequepoolstring_constructor starting.");
    DequeDequePoolString<> strings;    
    assertEqual(strings.size(), 0);

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_of_deque_dequedequepoolstring_push_front_push_back)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_of_deque_dequedequepoolstring_push_front_push_back starting.");
    DequeDequePoolString<> strings;
    PoolString<> string = "test";
    assertEqual(strings.size(), 0);

    assertTrue(strings.push_front());
    assertEqual(strings.size(), 1);

    assertTrue(strings.push_front());
    assertEqual(strings.size(), 2);

    assertTrue(strings.push_front());
    assertEqual(strings.size(), 3);

    assertTrue(strings.push_back(0, string));
    assertFalse(strings.push_back(3, string));

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_of_deque_dequedequepoolstring_size)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_of_deque_dequedequepoolstring_size starting.");
    DequeDequePoolString<> strings;    
    assertEqual(strings.size(), 0);
    assertEqual(strings.size(-1), -1);
    assertEqual(strings.size(0), -1);

    assertTrue(strings.push_front());
    assertEqual(strings.size(), 1);
    assertEqual(strings.size(0), 0);

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_of_deque_dequedequepoolstring_clear)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_of_deque_dequedequepoolstring_clear starting.");
    DequeDequePoolString<> strings;    
    PoolString<> string("test");

    assertTrue(strings.push_front());
    assertEqual(strings.size(), 1);
    assertEqual(strings.size(0), 0);

    assertTrue(strings.push_back(0, string));
    assertEqual(strings.size(0), 1);
    assertTrue(strings.clear(0));
    assertEqual(strings.size(0), 0);
    assertEqual(strings.size(), 1);
    assertTrue(strings.clear());
    assertEqual(strings.size(), 0);

    assertFalse(strings.clear(0));

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_of_deque_dequedequepoolstring_get_str_get_str_idx)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_of_deque_dequedequepoolstring_get_str_get_str_idx starting.");
    DequeDequePoolString<> strings;
    PoolString<> string("test");

    assertTrue(strings.push_front());
    assertTrue(strings.push_back(0, string));
    assertEqual(strings.get_str(0, 0).c_str(), string.c_str());
    assertEqual(get_stringpool(nullptr)->c_str(strings.get_str_idx(0, 0)), string.c_str());

    assertEqual(strings.get_str(-1, 0).c_str(), "");
    assertEqual(strings.get_str(1, 0).c_str(), "");
    assertEqual(strings.get_str(0, -1).c_str(), "");
    assertEqual(strings.get_str(0, 1).c_str(), "");

    assertEqual(strings.get_str_idx(-1, 0), -1);
    assertEqual(strings.get_str_idx(1, 0), -1);
    assertEqual(strings.get_str_idx(0, -1), -1);
    assertEqual(strings.get_str_idx(0, 1), -1);

    Test::min_verbosity = prevTestVerbosity;
}
