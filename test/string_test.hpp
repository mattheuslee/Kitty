#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>
#include <vector>

#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>

using namespace std;
using namespace kty;

test(string_stringpool)
{
    Log.notice(F("Test string_stringpool starting\n"));
    const int numStrings = 50;
    StringPool<numStrings, 20> stringPool;
    vector<int> stringPoolIndices;

    assertEqual(stringPool.available(), numStrings);
    assertEqual(stringPool.max_str_len(), 20);
    
    for (int i = 0; i < numStrings; ++i) {
        stringPoolIndices.push_back(stringPool.allocate_idx());
        assertMoreOrEqual(stringPoolIndices.back(), 0, "i = " << i);
    }
    assertEqual(stringPool.available(), 0);
    assertEqual(stringPool.allocate_idx(), -1);

    assertTrue(stringPool.inc_ref_count(stringPoolIndices[0]));
    assertTrue(stringPool.inc_ref_count(stringPoolIndices[0]));
    assertEqual(stringPool.num_ref(stringPoolIndices[0]), 3);
    assertTrue(stringPool.deallocate_idx(stringPoolIndices[0]));
    assertEqual(stringPool.num_ref(stringPoolIndices[0]), 2);
    assertTrue(stringPool.dec_ref_count(stringPoolIndices[0]));
    assertEqual(stringPool.num_ref(stringPoolIndices[0]), 1);

    for (int i = 0; i < numStrings; ++i) {
        assertTrue(stringPool.deallocate_idx(stringPoolIndices[i]));
        assertFalse(stringPool.deallocate_idx(stringPoolIndices[i]));
        stringPoolIndices[i] = -1;
    }
    assertEqual(stringPool.available(), numStrings);
    assertFalse(stringPool.deallocate_idx(-1));
    assertFalse(stringPool.inc_ref_count(-1));
    assertFalse(stringPool.dec_ref_count(-1));
    assertEqual(stringPool.num_ref(-1), -1);
    assertTrue(stringPool.c_str(-1) == nullptr);
}

test(string_poolstring)
{
    Log.notice(F("Test string_poolstring starting\n"));
    // Using global string pool
    PoolString<decltype(stringPool)> string1(stringPool), string2(stringPool);
    assertEqual(string1.strcmp(""), 0);
    assertEqual(string2.strcmp(""), 0);

    string1 = "1234567890";
    assertTrue(string1 == "1234567890");

    string1 = "12345678901234567890";
    assertTrue(string1 == "12345678901234567890");

    string1 = "1234567890123456789012345678901234567890";
    assertTrue(string1 == "1234567890123456789012345678901234567890");

    string1 = "12345678901234567890123456789012345678901";
    assertTrue(string1 == "1234567890123456789012345678901234567890");

    string2 = string1;
    assertTrue(string1 == "1234567890123456789012345678901234567890");
    
    PoolString<decltype(stringPool)> string3(string1);
    assertTrue(string3 == "1234567890123456789012345678901234567890");
    assertNotEqual(string1.pool_idx(), string3.pool_idx());

    string1 = "";
    string2 = "";
    assertEqual(string1.strcmp(""), 0);
    assertEqual(string2.strcmp(""), 0);

    string1.strcat("12345");
    assertTrue(string1 == "12345");

    string1.strcat("67890");
    assertTrue(string1 == "1234567890");

    string1 = "";
    assertTrue(string1 == "");

    string1 += "12345";
    assertTrue(string1 == "12345");

    string1 += "67890";
    assertTrue(string1 == "1234567890");
}

test(string_deque_of_poolstring)
{
    Log.notice(F("Test string_deque_of_poolstring starting\n"));
    // Using global alloc and string pool
    const int numStrings = 10;
    using poolstring_t = PoolString<decltype(stringPool)>;
    Deque<PoolString<decltype(stringPool)>, decltype(alloc)> strings(alloc);

    for (int i = 0; i < numStrings; ++i) {
        strings.push_back(poolstring_t(stringPool));
        assertTrue(strings[i] == "", "i = " << i);
        assertEqual(stringPool.num_ref(strings[i].pool_idx()), 1);
    }
    assertEqual(strings.size(), numStrings);

    char str[20] = "";
    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        strings[i] = str;
    }

    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        assertTrue(strings[i] == str, "i = " << i);
    }

    for (int i = 0; i < numStrings; ++i) {
        strings.pop_back();
    }
    assertEqual(strings.size(), 0);

    for (int i = 0; i < numStrings; ++i) {
        strings.push_front(poolstring_t(stringPool));
        assertTrue(strings[i] == "", "i = " << i);
        assertEqual(stringPool.num_ref(strings[i].pool_idx()), 1);
    }
    assertEqual(strings.size(), numStrings);

    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        strings[i] = str;
    }

    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        assertTrue(strings[i] == str, "i = " << i);
    }

    for (int i = 0; i < numStrings; ++i) {
        strings.pop_front();
    }
    assertEqual(strings.size(), 0);
}
