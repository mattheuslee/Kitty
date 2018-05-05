#pragma once

#include <kty/stl_impl.hpp>
#include <vector>

#include <kty/containers/allocator.hpp>

using namespace std;
using namespace kty;

test(allocator)
{
    const int numInts = 10;
    Allocator<numInts, sizeof(int)> alloc;
    vector<int*> ints;

    for (int i = 0; i < numInts; ++i) {
        ints.push_back((int*)alloc.allocate());
        *(ints[i]) = i;
    }
    assertTrue(alloc.allocate() == nullptr);

    for (int i = 0; i < numInts; ++i) {
        assertEqual(*(ints[i]), i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        alloc.deallocate(ints[i]);
        ints[i] = nullptr;
    }
}
