#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>

using namespace kty;

test(deque)
{
    Log.notice(F("Test deque starting\n"));
    const int maxSize = 10;
    Allocator<maxSize + 1, sizeof(int) * 8> alloc;
    Deque<int, decltype(alloc)> deque(alloc);

    for (int i = 0; i < maxSize; ++i) {
        deque.push_back(i);
    }
    for (int i = 0; i < maxSize; ++i) {
        assertEqual(deque[i], i, "i = " << i);
    }
    assertTrue(deque.size() == maxSize);
    for (int i = 0; i < maxSize; ++i) {
        deque.pop_back();
    }
    assertTrue(deque.is_empty());

    for (int i = 0; i < maxSize; ++i) {
        deque.push_front(maxSize - i - 1);
    }
    for (int i = 0; i < maxSize; ++i) {
        assertEqual(deque[i], i, "i = " << i);
    }
    assertTrue(deque.size() == maxSize);
    for (int i = 0; i < maxSize; ++i) {
        deque.pop_front();
    }
    assertTrue(deque.is_empty());

    deque.push_front(2);
    deque.push_front(1);
    assertEqual(deque.front(), 1);
    assertEqual(deque.back(), 2);
}