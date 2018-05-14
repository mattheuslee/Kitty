#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/sizes.hpp>

using namespace kty;

test(deque_constructors) {
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_constructors starting.");

    Deque<int> deque1;
    Deque<int> deque2(alloc);
    deque2.push_back(1);
    Deque<int> deque3(deque2);
    deque3 = deque2;

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_front) {
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_front starting.");

    const int numInts = 10;
    Allocator<numInts + 1, Sizes::alloc_block_size> alloc;
    Deque<int, decltype(alloc)> deque(alloc);
    assertNotEqual(deque.front(), -1);
    for (int i = 0; i < numInts; ++i) {
        assertTrue(deque.push_front(i), "i = " << i);
        assertEqual(deque.size(), i + 1, "i = " << i);
    }
    assertFalse(deque.push_front(0));

    for (int i = 0; i < numInts; ++i) {
        assertEqual(deque.front(), numInts - 1 - i, "i = " << i);
        assertTrue(deque.pop_front(), "i = " << i);
        assertEqual(deque.size(), numInts - 1 - i, "i = " << i);
    }
    assertFalse(deque.pop_front());

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_back) {
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_back starting.");
    const int numInts = 10;
    Allocator<numInts + 1, Sizes::alloc_block_size> alloc;
    Deque<int, decltype(alloc)> deque(alloc);
    assertNotEqual(deque.back(), -1);
    for (int i = 0; i < numInts; ++i) {
        assertTrue(deque.push_back(i), "i = " << i);
        assertEqual(deque.size(), i + 1, "i = " << i);
    }
    assertFalse(deque.push_back(0));

    for (int i = 0; i < numInts; ++i) {
        assertEqual(deque.back(), numInts - 1 - i, "i = " << i);
        assertTrue(deque.pop_back(), "i = " << i);
        assertEqual(deque.size(), numInts - 1 - i, "i = " << i);
    }
    assertFalse(deque.pop_back());

    Test::min_verbosity = prevTestVerbosity;
}

test(deque_iterators_and_access) {
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test deque_iterators_and_access starting.");
    const int numInts = 10;
    Deque<int> deque;
    assertNotEqual(deque[0], -1);
    for (int i = 0; i < numInts; ++i) {
        assertTrue(deque.push_back(i), "i = " << i);
    }
    int i = 0;
    for (Deque<int>::Iterator it = deque.begin(); it != deque.end(); ++it, ++i) {
        assertEqual(*it, i, "i = " << i);
    }
    i = numInts - 1;
    for (Deque<int>::Iterator it = deque.end(); it != deque.begin(); ) {
        --it;
        assertEqual(*it, i, "i = " << i);
        --i;
    }

    i = 0;
    for (Deque<int>::ConstIterator it = deque.cbegin(); it != deque.cend(); ++it, ++i) {
        assertEqual(*it, i, "i = " << i);
    }
    i = numInts - 1;
    for (Deque<int>::ConstIterator it = deque.cend(); it != deque.cbegin(); ) {
        --it;
        assertEqual(*it, i, "i = " << i);
        --i;
    }
    
    const Deque<int> deque2(deque);
    i = 0;
    for (Deque<int>::ConstIterator it = deque2.begin(); it != deque2.end(); ++it, ++i) {
        assertEqual(*it, i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        assertEqual(deque[i], i, "i = " << i);
    }
    
    Test::min_verbosity = prevTestVerbosity;
}

test(deque_erase) {
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test deque_iterators_and_access starting.");
    Deque<int> deque;

    assertEqual(deque.size(), 0);
    assertTrue(deque.push_back(1));
    assertTrue(deque.push_back(2));
    assertEqual(deque.size(), 2);
    assertTrue(deque.erase(1));
    assertEqual(deque.size(), 1);
    assertEqual(deque[0], 1);
    assertFalse(deque.erase(1));
    assertTrue(deque.erase(0));
    assertEqual(deque.size(), 0);

    assertTrue(deque.push_back(1));
    assertTrue(deque.push_back(2));
    Deque<int>::Iterator it = deque.begin();
    it = deque.erase(it);
    assertEqual(deque.size(), 1);
    assertEqual(deque[0], 2);
    assertEqual(*it, 2);
    it = deque.erase(it);
    assertEqual(deque.size(), 0);

    Test::min_verbosity = prevTestVerbosity;
}
