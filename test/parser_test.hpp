#pragma once

#include <kty/parser.hpp>
#include <kty/token.hpp>

using namespace kty;

void parser_check_tokens_match(Deque<Token<>> & generatedTokens, 
                               Deque<Token<>> & expectedTokens, 
                               char const * comment);

void parser_print_tokens(Deque<Token<>> const & tokens);

test(parser_constructors)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test parser_constructors starting.");
    Parser<> parser1;
    Parser<> parser2(Deque<Token<>>());
    Parser<> parser3(get_alloc, get_stringpool, Deque<Token<>>());

    Test::min_verbosity = prevTestVerbosity;
}

test(parser_parse_functions)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName("parser_parse_functions");

    Serial.println("Test parser_parse_functions starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command;
    Deque<Token<>> tokenizedCommand;

    command = "answer IsNumber(42)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "answer"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "42"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_NUM));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());

    command = "light IsLED(13, 25)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "13"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "25"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_LED));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());

    command = "blink IsGroup (";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_GROUP));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());

    command = "blink RunGroup(10)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "10"));
    expectedTokens.push_back(Token<>(TokenType::RUN_GROUP));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "light MoveByFor(100, 1000)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1000"));
    expectedTokens.push_back(Token<>(TokenType::MOVE_BY_FOR));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "light MoveBy(100)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::MOVE_BY));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "light SetToFor(0 + 100, 1000)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "0"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::MATH_ADD));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1000"));
    expectedTokens.push_back(Token<>(TokenType::SET_TO_FOR));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "light SetTo(100)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::SET_TO));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "If (answer < 100) (";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "answer"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::LESS));
    expectedTokens.push_back(Token<>(TokenType::IF));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "Else (";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::ELSE));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    command = "'hello!'";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::STRING, "hello!"));
    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());
    
    Test::min_verbosity = prevTestVerbosity;
}

test(parser_arithmetic_expression)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName(stringPool, "parser_arithmetic_expression");

    Serial.println("Test parser_arithmetic_expression starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command;
    Deque<Token<>> tokenizedCommand;

    command = "1 + 2 / 3 ^ 4 ^ 5 - 6 % 7 + 8 * - 9";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "2"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "3"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "4"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "5"));
    expectedTokens.push_back(Token<>(TokenType::MATH_POW));
    expectedTokens.push_back(Token<>(TokenType::MATH_POW));
    expectedTokens.push_back(Token<>(TokenType::MATH_DIV));
    expectedTokens.push_back(Token<>(TokenType::MATH_ADD));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "6"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "7"));
    expectedTokens.push_back(Token<>(TokenType::MATH_MOD));
    expectedTokens.push_back(Token<>(TokenType::MATH_SUB));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "8"));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "9"));
    expectedTokens.push_back(Token<>(TokenType::UNARY_NEG));
    expectedTokens.push_back(Token<>(TokenType::MATH_MUL));
    expectedTokens.push_back(Token<>(TokenType::MATH_ADD));

    tokenizedCommand = tokenizer.tokenize(command);
    parser.set_command(tokenizedCommand);
    generatedTokens = parser.parse();
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = parser.parse(tokenizedCommand);
    parser_check_tokens_match(generatedTokens, expectedTokens, (testName + "(parse) [" + command + "]").c_str());

    Test::min_verbosity = prevTestVerbosity;
}

void parser_check_tokens_match(Deque<Token<>> & generatedTokens, 
                               Deque<Token<>> & expectedTokens, 
                               char const * comment) {
    assertEqual(generatedTokens.size(), expectedTokens.size(), comment);
    Deque<Token<>>::Iterator genIter = generatedTokens.begin();
    Deque<Token<>>::Iterator expIter = expectedTokens.begin();
    int i = 0;
    while (genIter != generatedTokens.end() && expIter != expectedTokens.end()) {
        assertEqual(genIter->get_type(), expIter->get_type(), "i = " << i << ": " << comment);
        assertEqual(genIter->get_value().c_str(), expIter->get_value().c_str(), "i = " << i << ": " << comment);
        ++genIter;
        ++expIter;
        ++i;
    }
}

void parser_print_tokens(Deque<Token<>> const & tokens) {
    for (auto & token: tokens)
        Serial.println(token.str().c_str());
}
