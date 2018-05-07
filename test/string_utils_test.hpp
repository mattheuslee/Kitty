#pragma once

#include <kty/stl_impl.hpp>
#include <string>

#include <kty/containers/string.hpp>
#include <kty/string_utils.hpp>

using namespace std;
using namespace kty;

test(string_utils_str_to_int_empty_string)
{
    Log.notice(F("Test string_utils_str_to_int_empty_string starting\n"));
    assertEqual(str_to_int(""), 0);
}

test(string_utils_str_to_int_invalid_string)
{
    Log.notice(F("Test string_utils_str_to_int_invalid_string starting\n"));
    assertEqual(str_to_int("a"), 0);
}

test(string_utils_str_to_int_positive)
{
    Log.notice(F("Test string_utils_str_to_int_positive starting\n"));
    assertEqual(str_to_int("1"), 1);
    assertEqual(str_to_int("12"), 12);
    assertEqual(str_to_int("123"), 123);
    assertEqual(str_to_int("1234"), 1234);
    assertEqual(str_to_int("12345"), 12345);
}

test(string_utils_str_to_int_negative)
{
    Log.notice(F("Test string_utils_str_to_int_negative starting\n"));
    assertEqual(str_to_int("-1"), -1);
    assertEqual(str_to_int("-12"), -12);
    assertEqual(str_to_int("-123"), -123);
    assertEqual(str_to_int("-1234"), -1234);
    assertEqual(str_to_int("-12345"), -12345);
}

test(string_utils_int_to_str_positive)
{
    Log.notice(F("Test string_utils_int_to_str_positive starting\n"));
    assertTrue(int_to_str(1) == "1");
    assertTrue(int_to_str(12) == "12");
    assertTrue(int_to_str(123) == "123");
    assertTrue(int_to_str(1234) == "1234");
    assertTrue(int_to_str(12345) == "12345");
}

test(string_utils_int_to_str_negative)
{
    Log.notice(F("Test string_utils_int_to_str_negative starting\n"));
    assertTrue(int_to_str(-1) == "-1");
    assertTrue(int_to_str(-12) == "-12");
    assertTrue(int_to_str(-123) == "-123");
    assertTrue(int_to_str(-1234) == "-1234");
    assertTrue(int_to_str(-12345) == "-12345");
}

test(string_utils_int_to_str2_positive)
{
    Log.notice(F("Test string_utils_int_to_str2_positive starting\n"));
    // Use global string pool
    assertTrue(int_to_str(1, stringPool) == "1");
    assertTrue(int_to_str(12, stringPool) == "12");
    assertTrue(int_to_str(123, stringPool) == "123");
    assertTrue(int_to_str(1234, stringPool) == "1234");
    assertTrue(int_to_str(12345, stringPool) == "12345");
}

test(string_utils_int_to_str2_negative)
{
    Log.notice(F("Test string_utils_int_to_str2_negative starting\n"));
    assertTrue(int_to_str(-1, stringPool) == "-1");
    assertTrue(int_to_str(-12, stringPool) == "-12");
    assertTrue(int_to_str(-123, stringPool) == "-123");
    assertTrue(int_to_str(-1234, stringPool) == "-1234");
    assertTrue(int_to_str(-12345, stringPool) == "-12345");
}

test(string_utils_remove_str_whitespace)
{
    Log.notice(F("Test string_utils_remove_str_whitespace starting\n"));
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

test(string_utils_remove_str_whitespace2)
{
    Log.notice(F("Test string_utils_remove_str_whitespace2 starting\n"));
    PoolString<decltype(stringPool)> str(stringPool);
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
