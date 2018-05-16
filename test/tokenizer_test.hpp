#pragma once

#include <kty/containers/deque.hpp>
#include <kty/token.hpp>
#include <kty/tokenizer.hpp>

using namespace kty;

void tokenizer_check_tokens_match(Deque<Token<>> & generatedTokens, 
                                  Deque<Token<>> & expectedTokens, 
                                  char const * comment);

void tokenizer_print_tokens(Deque<Token<>> const & tokens);

test(tokenizer_process_math_tokens)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test tokenizer_process_math_tokens starting.");
    Deque<Token<>> tokens(alloc);
    tokens.push_back(Token<>(TokenType::LESS));
    tokens.push_back(Token<>(TokenType::EQUALS));
    tokenizer.process_math_tokens(tokens);
    assertEqual(tokens.front().get_type(), TokenType::L_EQUALS);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<>(TokenType::GREATER));
    tokens.push_back(Token<>(TokenType::EQUALS));
    tokenizer.process_math_tokens(tokens);
    assertEqual(tokens.front().get_type(), TokenType::G_EQUALS);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<>(TokenType::MATH_SUB));
    tokenizer.process_math_tokens(tokens);
    assertEqual(tokens.front().get_type(), TokenType::UNARY_NEG);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<>(TokenType::MATH_ADD));
    tokens.push_back(Token<>(TokenType::MATH_SUB));
    tokenizer.process_math_tokens(tokens);
    tokens.pop_front();
    assertEqual(tokens.front().get_type(), TokenType::UNARY_NEG);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<>(TokenType::OP_PAREN));
    tokens.push_back(Token<>(TokenType::MATH_SUB));
    tokenizer.process_math_tokens(tokens);
    tokens.pop_front();
    assertEqual(tokens.front().get_type(), TokenType::UNARY_NEG);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_get_additional_arguments)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test tokenizer_get_additional_arguments starting.");

    assertEqual(tokenizer.get_additional_arguments(TokenType::CREATE_NUM, 0).c_str(), "0");
    assertEqual(tokenizer.get_additional_arguments(TokenType::CREATE_NUM, 1).c_str(), "");

    assertEqual(tokenizer.get_additional_arguments(TokenType::CREATE_LED, 1).c_str(), ",50");
    assertEqual(tokenizer.get_additional_arguments(TokenType::CREATE_LED, 2).c_str(), "");
    
    assertEqual(tokenizer.get_additional_arguments(TokenType::RUN_GROUP, 0).c_str(), "1");
    assertEqual(tokenizer.get_additional_arguments(TokenType::RUN_GROUP, 1).c_str(), "");
    
    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_constructors)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test tokenizer_constructors starting.");
    Tokenizer<> tokenizer1;
    Tokenizer<> tokenizer2(PoolString<>());
    Tokenizer<> tokenizer3(get_alloc, get_stringpool, PoolString<>());

    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_unknown_token)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName(stringPool, "tokenizer_unknown_token");    
    
    Serial.println("Test tokenizer_unknown_token starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command(stringPool);

    command = "Blah";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());
    
    command = "$#";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());
    
    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_tokenize_missing_function_arguments)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName(stringPool, "tokenizer_tokenize_missing_function_arguments");    

    Serial.println("Test tokenizer_tokenize_missing_function_arguments starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command(stringPool);

    command = "answer IsNumber()";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "answer"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_NUM));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "0"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "answer IsNumber(42)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "answer"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_NUM));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "42"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED()";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_LED));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "13"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "50"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED(15)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_LED));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "15"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "50"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED(15, 25)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_LED));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "15"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "25"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup()";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::RUN_GROUP));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup(-1)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::RUN_GROUP));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::UNARY_NEG));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup(10)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::RUN_GROUP));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "10"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_tokenize_functions)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName(stringPool, "tokenizer_tokenize_functions");    

    Serial.println("Test tokenizer_tokenize_functions starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command(stringPool);

    command = "answer IsNumber(42)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "answer"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_NUM));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "42"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED(13, 25)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_LED));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "13"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "25"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink IsGroup (";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::CREATE_GROUP));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup(10)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "blink"));
    expectedTokens.push_back(Token<>(TokenType::RUN_GROUP));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "10"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light MoveByFor(100, 1000)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::MOVE_BY_FOR));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1000"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light MoveBy(100)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::MOVE_BY));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light SetToFor(100, 1000)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::SET_TO_FOR));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "1000"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light SetTo(100)";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::NAME, "light"));
    expectedTokens.push_back(Token<>(TokenType::SET_TO));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "Print(\"num is \", num, ' and num + 5 is ', (num + 5))";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::PRINT));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::STRING, "num is "));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::NAME, "num"));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::STRING, " and num + 5 is "));
    expectedTokens.push_back(Token<>(TokenType::COMMA));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NAME, "num"));
    expectedTokens.push_back(Token<>(TokenType::MATH_ADD));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "5"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "If (answer < 100) (";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::IF));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::NAME, "answer"));
    expectedTokens.push_back(Token<>(TokenType::LESS));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "100"));
    expectedTokens.push_back(Token<>(TokenType::CL_PAREN));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "Else (";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::ELSE));
    expectedTokens.push_back(Token<>(TokenType::OP_PAREN));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_tokenize_string)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName(stringPool, "tokenizer_tokenize_string");    

    Serial.println("Test tokenizer_tokenize_string starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command(stringPool);

    command = "'hello!'";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::STRING, "hello!"));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "\"hello!\"";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::STRING, "hello!"));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_tokenize_math_expression)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<> testName(stringPool, "tokenizer_tokenize_math_expression");    

    Serial.println("Test tokenizer_tokenize_math_expression starting.");
    Deque<Token<>> expectedTokens;
    Deque<Token<>> generatedTokens;
    PoolString<> command(stringPool);

    // 3 is to ensure we get sub and not unary neg
    command = "=<>+3-*/%^&|!~";
    expectedTokens.clear();
    expectedTokens.push_back(Token<>(TokenType::EQUALS));
    expectedTokens.push_back(Token<>(TokenType::LESS));
    expectedTokens.push_back(Token<>(TokenType::GREATER));
    expectedTokens.push_back(Token<>(TokenType::MATH_ADD));
    expectedTokens.push_back(Token<>(TokenType::NUM_VAL, "3"));
    expectedTokens.push_back(Token<>(TokenType::MATH_SUB));
    expectedTokens.push_back(Token<>(TokenType::MATH_MUL));
    expectedTokens.push_back(Token<>(TokenType::MATH_DIV));
    expectedTokens.push_back(Token<>(TokenType::MATH_MOD));
    expectedTokens.push_back(Token<>(TokenType::MATH_POW));
    expectedTokens.push_back(Token<>(TokenType::LOGI_AND));
    expectedTokens.push_back(Token<>(TokenType::LOGI_OR));
    expectedTokens.push_back(Token<>(TokenType::LOGI_XOR));
    expectedTokens.push_back(Token<>(TokenType::LOGI_NOT));
    expectedTokens.push_back(Token<>(TokenType::CMD_END));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    Test::min_verbosity = prevTestVerbosity;
}

void tokenizer_check_tokens_match(Deque<Token<>> & generatedTokens, 
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

void tokenizer_print_tokens(Deque<Token<>> const & tokens) {
    for (auto & token: tokens)
        Serial.println(token.str().c_str());
}
