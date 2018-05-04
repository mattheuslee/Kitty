#pragma once

#include <kty/stl_impl.hpp>
#include <vector>

#include <kty/containers/allocator.hpp>

using namespace std;
using namespace kty;

/*test(allocator)
{
    int numInts = 10;
    vector<int*> ints;

    for (int i = 0; i < numInts; ++i) {
        Serial.println(i);
        ints.push_back((int*)alloc.allocate());
        *(ints[i]) = i;
    }

    for (int i = 0; i < numInts; ++i) {
        Serial.println(i);
        assertTrue(*(ints[i]) == i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        Serial.println(i);
        alloc.deallocate(ints[i]);
        ints[i] = NULL;
    }
}*/
