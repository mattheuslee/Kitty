#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>

using namespace std;
using namespace kty;

test(string_stringpool)
{
    const int numStrings = 100;
    StringPool<numStrings, 20> stringPool;

    assertEqual(stringPool.max_str_len(), 20);
    
    for (int i = 0; i < numStrings; ++i) {
        assertMoreOrEqual(stringPool.allocate_idx(), 0, "i = " << i);
    }
    assertEqual(stringPool.allocate_idx(), -1);
}

test(string_stringpool_string_deque)
{
    const int numStrings = 10;
    StringPool<numStrings, 20> stringPool;

    StringDeque<decltype(alloc), decltype(stringPool)> strings(alloc, stringPool);
    for (int i = 0; i < numStrings; ++i) {
        strings.push_back(stringPool.allocate_idx());
    }
    assertEqual(stringPool.allocate_idx(), -1);

    char str[20] = "";
    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        stringPool.strcpy(i, str);
    }

    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        assertEqual(strcmp(stringPool.c_str(i), str), 0, "i = " << i);
    }
}

test(string_stringpool_poolstring)
{
    StringPool<2, 10> stringPool;

    PoolString<decltype(stringPool)> string1(stringPool), string2(stringPool);
    assertEqual(string1.strcmp(""), 0);
    assertEqual(string2.strcmp(""), 0);

    string1 = "1234567890";
    assertEqual(string1.strcmp("1234567890"), 0);

    string1 = "12345678901234567890";
    assertEqual(string1.strcmp("1234567890"), 0);

    string2 = string1;
    assertEqual(string2.strcmp("1234567890"), 0);

    string1 = "";
    string2 = "";
    assertEqual(string1.strcmp(""), 0);
    assertEqual(string2.strcmp(""), 0);

    string1.strcat("12345");
    assertEqual(string1.strcmp("12345"), 0);

    string1.strcat("67890");
    assertEqual(string1.strcmp("1234567890"), 0);

    string1 = "";
    assertTrue(string1 == "");

    string1 += "12345";
    assertTrue(string1 == "12345");

    string1 += "67890";
    assertTrue(string1 == "1234567890");
}
