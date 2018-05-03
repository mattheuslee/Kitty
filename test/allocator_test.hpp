#pragma once

#include <kty/stl_impl.hpp>
#include <vector>

#include <kty/containers/allocator.hpp>

using namespace std;
using namespace kty;

test(allocator)
{
    int numInts = 10;
    Allocator<int> allocator(numInts);

    vector<int*> ints;

    for (int i = 0; i < numInts; ++i) {
        ints.push_back(allocator.allocate());
        *(ints[i]) = i;
    }

    for (int i = 0; i < numInts; ++i) {
        assertTrue(*(ints[i]) == i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        allocator.deallocate(ints[i]);
        ints[i] = NULL;
    }
}
