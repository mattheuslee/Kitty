#pragma once

#include <kty/stl_impl.hpp>
#include <string>

#include <kty/string_utils.hpp>

using namespace std;
using namespace kty;

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

test(string_utils_int_to_str)
{
    assertTrue(int_to_str(1) == "1");
    assertTrue(int_to_str(12) == "12");
    assertTrue(int_to_str(123) == "123");
    assertTrue(int_to_str(1234) == "1234");
    assertTrue(int_to_str(12345) == "12345");
    assertTrue(int_to_str(-1) == "-1");
    assertTrue(int_to_str(-12) == "-12");
    assertTrue(int_to_str(-123) == "-123");
    assertTrue(int_to_str(-1234) == "-1234");
    assertTrue(int_to_str(-12345) == "-12345");
}

test(string_utils_remove_str_whitespace)
{
    string str;
    str = " a";
    remove_str_whitespace(str);
    assertTrue(str == "a");
    str = "a ";
    remove_str_whitespace(str);
    assertTrue(str == "a");
    str = "a b";
    remove_str_whitespace(str);
    assertTrue(str == "ab");
    str = " a   b";
    remove_str_whitespace(str);
    assertTrue(str == "ab");
    str = "a b  ";
    remove_str_whitespace(str);
    assertTrue(str == "ab");
    str = " a   b   ";
    remove_str_whitespace(str);
    assertTrue(str == "ab");
}
