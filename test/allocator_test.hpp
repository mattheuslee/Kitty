#pragma once

#include <kty/stl_impl.hpp>
#include <vector>

#include <kty/containers/allocator.hpp>

using namespace std;
using namespace kty;

test(allocator)
{
    Log.notice(F("Test allocator starting\n"));
    const int numInts = 10;
    Allocator<numInts, sizeof(int)> alloc;
    vector<int*> ints;

    for (int i = 0; i < numInts; ++i) {
        ints.push_back((int*)alloc.allocate());
        assertTrue(ints[i] != nullptr);
        *(ints[i]) = i;
    }
    assertTrue(alloc.allocate() == nullptr);

    for (int i = 0; i < numInts; ++i) {
        assertEqual(*(ints[i]), i, "i = " << i);
    }

    for (int i = 0; i < numInts; ++i) {
        assertTrue(alloc.deallocate(ints[i]), "i = " << i);
        assertFalse(alloc.deallocate(ints[i]), "i = " << i);
        ints[i] = nullptr;
    }

    assertFalse(alloc.deallocate((int*)nullptr));
}
