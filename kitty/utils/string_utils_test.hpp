#pragma once

#include <kitty/stl_impl.hpp>
#include <string>

#include <kitty/utils/string_utils.hpp>

using namespace std;
using namespace kitty;
using namespace kitty::utils;

test(string_utils_str_to_int)
{
    assertEqual(str_to_int("1"), 1);
    assertEqual(str_to_int("12"), 12);
    assertEqual(str_to_int("123"), 123);
    assertEqual(str_to_int("1234"), 1234);
    assertEqual(str_to_int("12345"), 12345);
    assertEqual(str_to_int("-1"), -1);
    assertEqual(str_to_int("-12"), -12);
    assertEqual(str_to_int("-123"), -123);
    assertEqual(str_to_int("-1234"), -1234);
    assertEqual(str_to_int("-12345"), -12345);
}

test(string_utils_remove_str_whitespace)
{
    assertTrue(remove_str_whitespace(" a") == "a");
    assertTrue(remove_str_whitespace("a ") == "a");
    assertTrue(remove_str_whitespace("a b") == "ab");
    assertTrue(remove_str_whitespace(" a b") == "ab");
    assertTrue(remove_str_whitespace("a b ") == "ab");
    assertTrue(remove_str_whitespace(" a b ") == "ab");
}
