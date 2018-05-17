#pragma once

#include <kty/utils.hpp>

using namespace kty;

test(utils_swap) {
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test utils_swap starting.");
    int a = 5, b = 10;
    swap(a, b);
    assertEqual(a, 10);
    assertEqual(b, 5);

    Test::min_verbosity = prevTestVerbosity;
}
