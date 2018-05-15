#pragma once

#include <kty/stl_impl.hpp>
#include <string>

#include <kty/containers/string.hpp>
#include <kty/string_utils.hpp>

using namespace std;
using namespace kty;

test(string_utils_str_to_int)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_utils_str_to_int starting.");
    PoolString<> str(stringPool);
    str = "";
    assertEqual(str_to_int(str), 0);
    str = "b";
    assertEqual(str_to_int(str), 0);
    str = "0x";
    assertEqual(str_to_int(str), 0);
    str = "0";
    assertEqual(str_to_int(str), 0);
    str = "1";
    assertEqual(str_to_int(str), 1);
    str = "12";
    assertEqual(str_to_int(str), 12);
    str = "123";
    assertEqual(str_to_int(str), 123);
    str = "1234";
    assertEqual(str_to_int(str), 1234);
    str = "12345";
    assertEqual(str_to_int(str), 12345);
    str = "-1";
    assertEqual(str_to_int(str), -1);
    str = "-12";
    assertEqual(str_to_int(str), -12);
    str = "-123";
    assertEqual(str_to_int(str), -123);
    str = "-1234";
    assertEqual(str_to_int(str), -1234);
    str = "-12345";
    assertEqual(str_to_int(str), -12345);

    Test::min_verbosity = prevTestVerbosity;
}

test(string_utils_int_to_str)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_utils_int_to_str starting.");
    assertEqual(int_to_str(1, stringPool).c_str(), "1");
    assertEqual(int_to_str(12, stringPool).c_str(), "12");
    assertEqual(int_to_str(123, stringPool).c_str(), "123");
    assertEqual(int_to_str(1234, stringPool).c_str(), "1234");
    assertEqual(int_to_str(12345, stringPool).c_str(), "12345");
    assertEqual(int_to_str(-1, stringPool).c_str(), "-1");
    assertEqual(int_to_str(-12, stringPool).c_str(), "-12");
    assertEqual(int_to_str(-123, stringPool).c_str(), "-123");
    assertEqual(int_to_str(-1234, stringPool).c_str(), "-1234");
    assertEqual(int_to_str(-12345, stringPool).c_str(), "-12345");

    Test::min_verbosity = prevTestVerbosity;
}

test(string_utils_remove_str_whitespace)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_utils_remove_str_whitespace starting.");
    PoolString<> str;
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

    Test::min_verbosity = prevTestVerbosity;
}

test(string_utils_remove_str_multiple_whitespace)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_utils_remove_str_multiple_whitespace starting.");
    PoolString<> str;
    str = "  a";
    remove_str_multiple_whitespace(str);
    assertEqual(str.c_str(), " a");
    str = "a  ";
    remove_str_multiple_whitespace(str);
    assertEqual(str.c_str(), "a ");
    str = "a  b";
    remove_str_multiple_whitespace(str);
    assertEqual(str.c_str(), "a b");
    str = "  a   b";
    remove_str_multiple_whitespace(str);
    assertEqual(str.c_str(), " a b");
    str = "a  b  ";
    remove_str_multiple_whitespace(str);
    assertEqual(str.c_str(), "a b ");
    str = " a   b   ";
    remove_str_multiple_whitespace(str);
    assertEqual(str.c_str(), " a b ");

    Test::min_verbosity = prevTestVerbosity;
}
