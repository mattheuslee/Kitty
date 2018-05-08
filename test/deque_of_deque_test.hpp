#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>

using namespace kty;

test(deque_of_deque_dequedequepoolstring)
{
    Log.notice(F("Test deque_of_deque_dequedequepoolstring starting\n"));
    // Use global allocator and string pool
    DequeDequePoolString<decltype(stringPool), decltype(alloc)> strings(stringPool, alloc);
    PoolString<decltype(stringPool)> str(stringPool);

    assertEqual(strings.size(), 0);
    assertEqual(strings.size(0), -1);
    assertTrue(strings.get_str(0, 0) == "");
    strings.clear(0); // Should give undefined behaviour warnings

    strings.push_front();
    assertEqual(strings.size(), 1);
    assertEqual(strings.size(0), 0);
    assertTrue(strings.get_str(0, 0) == "");
    str = "12345";
    strings.push_back(0, str);
    strings.push_back(1, str); // Should give undefined behaviour warnings
    assertEqual(strings.size(0), 1);
    assertEqual(strings.get_str(0, 0).strlen(), 5);
    assertTrue(strings.get_str(0, 0) == "12345");

    strings.push_front();
    assertEqual(strings.size(), 2);
    assertEqual(strings.size(0), 0);
    str = "67890"; 
    strings.push_back(0, str);
    strings.push_back(1, str);
    assertEqual(strings.size(1), 2);
    assertEqual(strings.get_str(1, 0).strlen(), 5);
    assertEqual(strings.get_str(1, 1).strlen(), 5);
    assertTrue(strings.get_str(1, 0) == "12345");
    assertTrue(strings.get_str(1, 1) == "67890");
    assertEqual(strings.size(0), 1);
    assertEqual(strings.get_str(0, 0).strlen(), 5);
    assertTrue(strings.get_str(0, 0) == "67890");

    strings.clear(0);
    assertEqual(strings.size(0), 0);
    assertEqual(strings.size(1), 2);
    assertEqual(strings.get_str_idx(0, 0), -1); // Invalid i
    assertEqual(strings.get_str_idx(1, 2), -1); // Invalid j

    strings.clear(1);
    assertEqual(strings.size(0), 0);
    assertEqual(strings.size(1), 0);
}
