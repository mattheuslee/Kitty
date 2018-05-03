#pragma once

#include <kty/containers/deque.hpp>

using namespace kty;

test(deque)
{
    int maxSize = 10;
    Deque<int> deque(maxSize);
    for (int i = 0; i < maxSize; ++i) {
        deque.push_back(i);
    }
    for (int i = 0; i < maxSize; ++i) {
        assertTrue(deque[i] == i, "i = " << i);
    }
    for (int i = 0; i < maxSize; ++i) {
        deque.pop_back();
    }
    assertTrue(deque.is_empty());

    for (int i = 0; i < maxSize; ++i) {
        deque.push_front(maxSize - i - 1);
    }
    for (int i = 0; i < maxSize; ++i) {
        assertTrue(deque[i] == i, "i = " << i);
    }
    for (int i = 0; i < maxSize; ++i) {
        deque.pop_front();
    }
    assertTrue(deque.is_empty());

    deque.push_front(2);
    deque.push_front(1);
    assertTrue(deque.front() == 1);
    assertTrue(deque.back() == 2);
}
