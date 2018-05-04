#pragma once

#include <kty/stl_impl.hpp>
#include <vector>

#include <kty/containers/allocator.hpp>

using namespace std;
using namespace kty;

test(allocator)
{
    const int numInts = 10;
    Allocator<numInts, sizeof(int)> allocator;
    vector<int*> ints;

    for (int i = 0; i < numInts; ++i) {
        ints.push_back((int*)allocator.allocate());
        *(ints[i]) = i;
    }
    assertTrue(allocator.allocate() == NULL);

    for (int i = 0; i < numInts; ++i) {
        assertTrue(*(ints[i]) == i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        allocator.deallocate(ints[i]);
        ints[i] = NULL;
    }
}
