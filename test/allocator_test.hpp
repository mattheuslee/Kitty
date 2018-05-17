#pragma once

#include <kty/containers/deque.hpp>
#include <kty/containers/allocator.hpp>

using namespace kty;

test(allocator_constructor)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test allocator_constructor starting.");
    Allocator<1, 1> allocator;

    Test::min_verbosity = prevTestVerbosity;
}

test(allocator_stat_addresses)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test allocator_stat_addresses starting.");
    alloc.stat();
    alloc.reset_stat();
    alloc.dump_addresses();

    Test::min_verbosity = prevTestVerbosity;
}

test(allocator_available)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test allocator_available starting.");
    Allocator<10, 10> allocator;
    Deque<void *> addresses;
    assertEqual(allocator.available(), 10);
    for (int i = 0; i < 10; ++i) {
        assertTrue(addresses.push_back(allocator.allocate()));
        assertEqual(allocator.available(), 10 - 1 - i, "i = " << i);
    }

    for (int i = 0; i < 10; ++i) {
        assertTrue(allocator.deallocate(addresses.front()));
        assertEqual(allocator.available(), i + 1, "i = " << i);
        assertTrue(addresses.pop_front());
    }

    Test::min_verbosity = prevTestVerbosity;
}

test(allocator_get_addr_idx)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test allocator_get_addr_idx starting.");
    Allocator<1, 1> allocator;
    void * addr = allocator.allocate();
    int idx = allocator.get_idx(addr);

    assertTrue(allocator.get_addr(idx) == addr);
    assertEqual(allocator.get_idx(addr), idx);

    Test::min_verbosity = prevTestVerbosity;
}

test(allocator_ref_count)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test allocator_ref_count starting.");
    Allocator<1, 1> allocator;
    void * addr = allocator.allocate();
    assertEqual(allocator.ref_count(addr), 1);
    assertEqual(allocator.inc_ref_count(addr), 2);
    assertEqual(allocator.ref_count(addr), 2);
    assertEqual(allocator.dec_ref_count(addr), 1);
    assertEqual(allocator.ref_count(addr), 1);
    assertEqual(allocator.inc_ref_count(addr), 2);
    assertTrue(allocator.deallocate(addr));
    assertEqual(allocator.ref_count(addr), 1);
    assertTrue(allocator.deallocate(addr));
    assertEqual(allocator.ref_count(addr), 0);

    assertEqual(allocator.inc_ref_count(addr - 1), -1);
    assertEqual(allocator.ref_count(addr - 1), -1);
    assertEqual(allocator.dec_ref_count(addr - 1), -1);
    assertEqual(allocator.ref_count(addr - 1), -1);

    Test::min_verbosity = prevTestVerbosity;
}

test(allocator_allocate_deallocate)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test allocator_allocate_deallocate starting.");
    const int numInts = 10;
    Allocator<numInts, sizeof(int)> intAlloc;
    Deque<int*> ints;

    for (int i = 0; i < numInts; ++i) {
        assertTrue(ints.push_back((int *)(intAlloc.allocate())));
        assertTrue(ints[i] != nullptr, "i = " << i);
        *(ints[i]) = i;
    }
    assertTrue(intAlloc.allocate() == nullptr);

    for (int i = 0; i < numInts; ++i) {
        assertEqual(*(ints[i]), i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        assertTrue(intAlloc.deallocate(ints[i]), "i = " << i);
        assertFalse(intAlloc.deallocate(ints[i]), "i = " << i);
    }
    assertFalse(intAlloc.deallocate(ints[0] - 1));

    Test::min_verbosity = prevTestVerbosity;
}
