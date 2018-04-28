#pragma once

#include <kitty/stl_impl.hpp>
#include <string>
#include <utility>
#include <vector>

#include <kitty/parser/parser.hpp>
#include <kitty/tokenizer/tokenizer.hpp>

using namespace std;
using namespace kitty;
using namespace kitty::tokenizer;
using namespace kitty::parser;

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

test(parser_run_shunting_yard_arithmetic_expression)
{
    string command = "3 + 4 * 2 / (1 - 5)^2^3";
    Tokenizer tokenizer(command);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    tokens = parser.parse();
    static const vector<Token> expectedTokens = {
        {NUM_VAL, "3"}, 
        {NUM_VAL, "4"}, 
        {NUM_VAL, "2"}, 
        {MATH_MUL}, 
        {NUM_VAL, "1"}, 
        {NUM_VAL, "5"}, 
        {MATH_SUB}, 
        {NUM_VAL, "2"}, 
        {NUM_VAL, "3"}, 
        {MATH_POW}, 
        {MATH_POW}, 
        {MATH_DIV}, 
        {MATH_ADD}, 
    };
    parser_check_correct_tokens(tokens, expectedTokens, command);
}

test(parser_run_shunting_yard_is_number)
{
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

test(parser_run_shunting_yard_is_led)
{
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

test(parser_run_shunting_yard_is_servo)
{
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

test(parser_run_shunting_yard_is_group)
{
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

test(parser_run_shunting_yard_if)
{
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

test(parser_run_shunting_yard_else)
{
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

test(parser_run_shunting_yard_while)
{
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

test(parser_run_shunting_yard_move_by)
{
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

test(parser_run_shunting_yard_set_to)
{
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

test(parser_run_shunting_yard_move_by_for)
{
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

test(parser_run_shunting_yard_set_to_for)
{
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
