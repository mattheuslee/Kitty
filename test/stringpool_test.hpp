#pragma once

#include <kty/containers/deque.hpp>
#include <kty/containers/stringpool.hpp>

using namespace kty;

test(stringpool_stat_addresses)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test stringpool_stat_addresses starting.");
    stringPool.stat();
    stringPool.reset_stat();
    stringPool.dump_addresses();

    Test::min_verbosity = prevTestVerbosity;    
}

test(stringpool_available)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test stringpool_available starting.");
    StringPool<10, 10> stringPool;
    Deque<int> indices;

    assertEqual(stringPool.max_str_len(), 10);
    for (int i = 0; i < 10; ++i) {
        assertTrue(indices.push_back(stringPool.allocate_idx()));
        assertEqual(stringPool.available(), 10 - 1 - i, "i = " << i);
    }

    for (int i = 0; i < 10; ++i) {
        assertTrue(stringPool.deallocate_idx(indices.front()));
        assertEqual(stringPool.available(), i + 1, "i = " << i);
        assertTrue(indices.pop_front());
    }

    Test::min_verbosity = prevTestVerbosity;    
}

test(stringpool_ref_count)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test stringpool_available starting.");
    StringPool<1, 1> stringPool;
    int idx = stringPool.allocate_idx();
    
    assertEqual(stringPool.ref_count(idx), 1);
    assertEqual(stringPool.inc_ref_count(idx), 2);
    assertEqual(stringPool.ref_count(idx), 2);
    assertEqual(stringPool.dec_ref_count(idx), 1);
    assertEqual(stringPool.ref_count(idx), 1);
    assertEqual(stringPool.inc_ref_count(idx), 2);
    assertTrue(stringPool.deallocate_idx(idx));
    assertEqual(stringPool.ref_count(idx), 1);
    assertTrue(stringPool.deallocate_idx(idx));
    assertEqual(stringPool.ref_count(idx), 0);

    assertEqual(stringPool.inc_ref_count(-1), -1);
    assertEqual(stringPool.ref_count(-1), -1);
    assertEqual(stringPool.dec_ref_count(-1), -1);
    assertEqual(stringPool.ref_count(-1), -1);
    assertEqual(stringPool.inc_ref_count(1), -1);
    assertEqual(stringPool.ref_count(1), -1);
    assertEqual(stringPool.dec_ref_count(1), -1);
    assertEqual(stringPool.ref_count(1), -1);

    Test::min_verbosity = prevTestVerbosity;    
}

test(stringpool_allocate_deallocate_idx)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test stringpool_available starting.");
    const int numStrings = 10;    
    StringPool<numStrings, 1> stringPool;
    Deque<int> stringIndices;

    for (int i = 0; i < numStrings; ++i) {
        assertTrue(stringIndices.push_back(stringPool.allocate_idx()));
        assertNotEqual(stringIndices[i], -1, "i = " << i);
        assertTrue(stringPool.c_str(stringIndices[i]) != nullptr, "i = " << i);
    }
    assertEqual(stringPool.allocate_idx(), -1);

    for (int i = 0; i < numStrings; ++i) {
        assertTrue(stringPool.deallocate_idx(stringIndices[i]), "i = " << i);
        assertFalse(stringPool.deallocate_idx(stringIndices[i]), "i = " << i);
    }
    assertFalse(stringPool.deallocate_idx(-1));
    assertFalse(stringPool.deallocate_idx(numStrings));
    assertTrue(stringPool.c_str(-1) == nullptr);
    assertTrue(stringPool.c_str(numStrings) == nullptr);

    Test::min_verbosity = prevTestVerbosity;    
}
