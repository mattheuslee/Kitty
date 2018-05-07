#pragma once

#include <kty/stl_impl.hpp>
#include <string>
#include <utility>
#include <vector>

#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace std;
using namespace kty;

void parser_check_correct_types_values(vector<Token> const & tokens, vector<TokenType> const & expectedTypes, vector<string> const & expectedValues, string const & comment) {
    for (int i = 0; i < tokens.size(); ++i) {
        //Serial.println(tokens[i].str().c_str());
        assertTrue(tokens[i].type == expectedTypes[i], comment.c_str() << ": i = " << i);
        assertTrue(tokens[i].value == expectedValues[i], comment.c_str() << ": i = " << i);
    }
}

void parser_check_correct_tokens(vector<Token> const & tokens, vector<Token> const & expectedTokens, string const & comment) {
    for (int i = 0; i < tokens.size(); ++i) {
        //Serial.println(tokens[i].str().c_str());
        assertTrue(tokens[i].type == expectedTokens[i].type, comment.c_str() << ": i = " << i);
        assertTrue(tokens[i].value == expectedTokens[i].value, comment.c_str() << ": i = " << i);
    }
}

void print_tokens(vector<Token> const & tokens) {
    for (auto & token: tokens)
        Serial.println(token.str().c_str());
}

test(parser_arithmetic_expression)
{
    Log.notice(F("Test parser_arithmetic_expression starting\n"));
    string command = "3 + (1 - 5)^2^3 / 4";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    //print_tokens(tokens);
    static const vector<Token> expectedTokens = {
        {NUM_VAL, "3"}, 
        {NUM_VAL, "1"}, 
        {NUM_VAL, "5"}, 
        {MATH_SUB}, 
        {NUM_VAL, "2"}, 
        {NUM_VAL, "3"}, 
        {MATH_POW}, 
        {MATH_POW}, 
        {NUM_VAL, "4"}, 
        {MATH_DIV}, 
        {MATH_ADD}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);
}

test(parser_is_number)
{
    Log.notice(F("Test parser_is_number starting\n"));
    string command = "answer IsNumber(42)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "answer"}, 
        {NUM_VAL, "42"}, 
        {CREATE_NUM}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);
}

test(parser_is_led)
{
    Log.notice(F("Test parser_is_led starting\n"));
    string command = "light IsLED(13, 25)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "light"}, 
        {NUM_VAL, "13"}, 
        {NUM_VAL, "25"}, 
        {CREATE_LED}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_is_servo)
{
    Log.notice(F("Test parser_is_servo starting\n"));
    string command = "sweeper IsServo(10, 45)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "sweeper"}, 
        {NUM_VAL, "10"}, 
        {NUM_VAL, "45"}, 
        {CREATE_SERVO}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_is_group)
{
    Log.notice(F("Test parser_is_group starting\n"));
    string command = "blink IsGroup (";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "blink"}, 
        {CREATE_GROUP, ""}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_if)
{
    Log.notice(F("Test parser_if starting\n"));
    string command = "If (num % 3 = 0 & num % 5 = 0) (";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "num"}, 
        {NUM_VAL, "3"}, 
        {MATH_MOD}, 
        {NUM_VAL, "0"}, 
        {EQUALS}, 
        {NAME, "num"}, 
        {NUM_VAL, "5"}, 
        {MATH_MOD}, 
        {NUM_VAL, "0"}, 
        {EQUALS}, 
        {LOGI_AND}, 
        {IF}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_else)
{
    Log.notice(F("Test parser_else starting\n"));
    string command = "Else (";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {ELSE}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_while)
{
    Log.notice(F("Test parser_while starting\n"));
    string command = "While (val <= 20) (";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "val"}, 
        {NUM_VAL, "20"}, 
        {L_EQUALS}, 
        {WHILE}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_move_by)
{
    Log.notice(F("Test parser_move_by starting\n"));
    string command = "answer MoveBy(-10)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "answer"}, 
        {NUM_VAL, "10"}, 
        {UNARY_NEG}, 
        {MOVE_BY}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_set_to)
{
    Log.notice(F("Test parser_set_to starting\n"));
    string command = "light SetTo(75)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "light"}, 
        {NUM_VAL, "75"}, 
        {SET_TO}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_move_by_for)
{
    Log.notice(F("Test parser_move_by_for starting\n"));
    string command = "answer MoveByFor(-10, 1)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "answer"}, 
        {NUM_VAL, "10"}, 
        {UNARY_NEG}, 
        {NUM_VAL, "1"}, 
        {MOVE_BY_FOR}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}

test(parser_set_to_for)
{
    Log.notice(F("Test parser_set_to_for starting\n"));
    string command = "light SetToFor(75, 1)";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NAME, "light"}, 
        {NUM_VAL, "75"}, 
        {NUM_VAL, "1"}, 
        {SET_TO_FOR}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);   
}
