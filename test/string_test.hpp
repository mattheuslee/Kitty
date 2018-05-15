#pragma once

#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/sizes.hpp>

using namespace kty;

test(string_constructors)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_constructors starting.");
    PoolString<> string1(stringPool);
    assertEqual(string1.c_str(), "");

    PoolString<> string2(stringPool, 1);
    assertEqual(string2.pool_idx(), 1);

    PoolString<> string3(stringPool, "test");
    assertEqual(string3.c_str(), "test");

    PoolString<> string4;
    assertEqual(string4.c_str(), "");

    PoolString<> string5(1);
    assertEqual(string5.pool_idx(), 1);

    PoolString<> string6(get_stringpool);
    assertEqual(string6.c_str(), "");

    PoolString<> string7(get_stringpool, 1);
    assertEqual(string7.pool_idx(), 1);

    PoolString<> string8(-1, get_stringpool);
    assertEqual(string8.c_str(), "");

    PoolString<> string9(1, get_stringpool);
    assertEqual(string9.pool_idx(), 1);

    PoolString<> string10(get_stringpool, "test");
    assertEqual(string10.c_str(), "test");

    PoolString<> string11("testing");
    assertEqual(string11.c_str(), "testing");

    PoolString<> string12(string11);
    assertEqual(string12.c_str(), "testing");

    string12 = string1;
    assertEqual(string12.c_str(), "");

    string12 = string10;
    assertEqual(string12.c_str(), "test");
    
    Test::min_verbosity = prevTestVerbosity;    
}

test(string_max_length)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_max_length starting.");
    char longer[Sizes::string_length + 10 + 1] = "";
    for (int i = 0; i < Sizes::string_length + 10; ++i){
        longer[i] = 'a';
    }
    char maximum[Sizes::string_length + 1] = "";
    for (int i = 0; i < Sizes::string_length; ++i){
        maximum[i] = 'a';
    }

    PoolString<> string1(stringPool, longer);
    assertEqual(string1.c_str(), maximum);

    PoolString<> string2(get_stringpool, longer);
    assertEqual(string2.c_str(), maximum);
    
    PoolString<> string3(longer);
    assertEqual(string3.c_str(), maximum);
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_strcpy)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_strcpy starting.");
    PoolString<> string1;
    assertEqual(string1.c_str(), "");

    string1.strcpy("hello");
    assertEqual(string1.c_str(), "hello");

    string1.strcpy("hi");
    assertEqual(string1.c_str(), "hi");

    string1.strcpy("");
    assertEqual(string1.c_str(), "");

    PoolString<> string2(stringPool);
    assertEqual(string2.c_str(), "");

    string2.strcpy("hello");
    assertEqual(string2.c_str(), "hello");

    string2.strcpy("hi");
    assertEqual(string2.c_str(), "hi");

    string2.strcpy("");
    assertEqual(string2.c_str(), "");
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_strcat)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_strcat starting.");
    PoolString<> string1;
    assertEqual(string1.c_str(), "");

    string1.strcat("hello");
    assertEqual(string1.c_str(), "hello");

    string1.strcat("hi");
    assertEqual(string1.c_str(), "hellohi");

    string1.strcat("");
    assertEqual(string1.c_str(), "hellohi");

    PoolString<> string2(stringPool);
    assertEqual(string2.c_str(), "");

    string2.strcat("hello");
    assertEqual(string2.c_str(), "hello");

    string2.strcat("hi");
    assertEqual(string2.c_str(), "hellohi");

    string2.strcat("");
    assertEqual(string2.c_str(), "hellohi");
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_strcmp)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_strcmp starting.");
    PoolString<> string("5");
    assertEqual(string.strcmp("5"), 0);
    assertTrue(string.strcmp("6") < 0);
    assertTrue(string.strcmp("4") > 0);
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_strlen)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_strlen starting.");
    PoolString<> string;
    assertEqual(string.strlen(), 0);

    string.strcat("hello");
    assertEqual(string.strlen(), 5);

    string.strcat("hi");
    assertEqual(string.strlen(), 7);
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_equality_comparison_operator)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_equality_comparison_operator starting.");
    PoolString<> string1;
    assertTrue(string1 == "");

    string1 = "hello";
    assertTrue(string1 == "hello");

    PoolString<> string2("hello");
    assertTrue(string1 == string2);
    assertTrue(string2 == string1);
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_add_operator)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_add_operator starting.");
    PoolString<> string1;
    assertEqual(string1.c_str(), "");
    assertEqual((string1 + "hello").c_str(), "hello");

    PoolString<> string2("hello");
    assertEqual((string1 + string2).c_str(), string2.c_str());
    assertEqual((string1 + string2).c_str(), "hello");
    assertEqual((string2 + string1).c_str(), string2.c_str());
    assertEqual((string2 + string1).c_str(), "hello");
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_add_equals_operator)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_add_equals_operator starting.");
    PoolString<> string1;
    assertEqual(string1.c_str(), "");
    assertEqual((string1 += "hello").c_str(), "hello");
    assertEqual(string1.c_str(), "hello");
    string1 = "";

    PoolString<> string2("hello");
    assertEqual((string1 += string2).c_str(), "hello");
    assertEqual(string1.c_str(), "hello");
    
    Test::min_verbosity = prevTestVerbosity;
}

test(string_index_operator)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test string_index_operator starting.");
    PoolString<> string1;
    char str[2] = "";
    for (int i = 0; i < 10; ++i) {
        str[0] = '0' + i;
        string1 += str;
    }
    for (int i = 0; i < 10; ++i) {
        assertEqual(string1[i], '0' + i, "i = " << i);
    }

    const PoolString<> string2(string1);
    for (int i = 0; i < 10; ++i) {
        assertEqual(string2[i], '0' + i, "i = " << i);
    }

    Test::min_verbosity = prevTestVerbosity;
}

test(string_find)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test string_find starting.");
    const PoolString<> string("abcdefghijklmnopqrstuvwxyz");
    for (int i = 0; i < 26; ++i) {
        assertEqual(string.find('a' + i), i, "i = " << i);
        assertEqual(string.find('a' + i, i), i, "i = " << i);
        assertEqual(string.find('a' + i, i + 1), -1, "i = " << i);
    }
    assertEqual(string.find('1'), -1);

    assertEqual(string.find("abcde"), 0);
    assertEqual(string.find("abcde", 0), 0);
    assertEqual(string.find("abcde", 1), -1);

    assertEqual(string.find("xyz"), 23);
    assertEqual(string.find("xyz", 23), 23);
    assertEqual(string.find("xyz", 24), -1);

    Test::min_verbosity = prevTestVerbosity;
}

test(string_count)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_count starting.");
    const PoolString<> string("12345123451234512345");
    assertEqual(string.count('1'), 4);
    assertEqual(string.count('2'), 4);
    assertEqual(string.count('3'), 4);
    assertEqual(string.count('4'), 4);
    assertEqual(string.count('5'), 4);

    assertEqual(string.count('1', 5), 3);
    assertEqual(string.count('2', 5), 3);
    assertEqual(string.count('3', 5), 3);
    assertEqual(string.count('4', 5), 3);
    assertEqual(string.count('5', 5), 3);

    assertEqual(string.count('1', 5, 15), 2);
    assertEqual(string.count('2', 5, 15), 2);
    assertEqual(string.count('3', 5, 15), 2);
    assertEqual(string.count('4', 5, 15), 2);
    assertEqual(string.count('5', 5, 15), 2);

    assertEqual(string.count('a'), 0);

    Test::min_verbosity = prevTestVerbosity;
}

test(string_insert)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_insert starting.");
    PoolString<> string("");

    string.insert("hello");
    assertEqual(string.c_str(), "hello");

    string.insert("hi");
    assertEqual(string.c_str(), "hihello");

    string.insert("hey", 2);
    assertEqual(string.c_str(), "hiheyhello");

    string.insert("yo", 10);
    assertEqual(string.c_str(), "hiheyhelloyo");

    Test::min_verbosity = prevTestVerbosity;
}

test(string_substr_il)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_substr_il starting.");
    const PoolString<> string("1234567890");

    assertEqual(string.substr_il().c_str(), "1234567890");
    assertEqual(string.substr_il(0).c_str(), "1234567890");
    assertEqual(string.substr_il(0, 10).c_str(), "1234567890");

    assertEqual(string.substr_il(2).c_str(), "34567890");    
    assertEqual(string.substr_il(2, 8).c_str(), "34567890");    
    assertEqual(string.substr_il(2, 6).c_str(), "345678");    

    Test::min_verbosity = prevTestVerbosity;
}

test(string_substr_ii)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test string_substr_ii starting.");
    const PoolString<> string("1234567890");

    assertEqual(string.substr_ii().c_str(), "1234567890");
    assertEqual(string.substr_ii(0).c_str(), "1234567890");
    assertEqual(string.substr_ii(0, 10).c_str(), "1234567890");

    assertEqual(string.substr_ii(2).c_str(), "34567890");    
    assertEqual(string.substr_ii(2, 8).c_str(), "345678");    
    assertEqual(string.substr_ii(2, 6).c_str(), "3456");    

    Test::min_verbosity = prevTestVerbosity;
}
