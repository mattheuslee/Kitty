#pragma once

#include <kty/containers/deque.hpp>
#include <kty/token.hpp>
#include <kty/tokenizer.hpp>

using namespace kty;

void tokenizer_check_tokens_match(Deque<Token<stringpool_t>, alloc_t> & generatedTokens, 
                                  Deque<Token<stringpool_t>, alloc_t> & expectedTokens, 
                                  char const * comment);

test(tokenizer_process_math_tokens)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test tokenizer_process_math_tokens starting.");
    Deque<Token<stringpool_t>, alloc_t> tokens(alloc);
    tokens.push_back(Token<stringpool_t>(TokenType::LESS, stringPool));
    tokens.push_back(Token<stringpool_t>(TokenType::EQUALS, stringPool));
    tokenizer.process_math_tokens(tokens);
    assertEqual(tokens.front().get_type(), TokenType::L_EQUALS);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<stringpool_t>(TokenType::GREATER, stringPool));
    tokens.push_back(Token<stringpool_t>(TokenType::EQUALS, stringPool));
    tokenizer.process_math_tokens(tokens);
    assertEqual(tokens.front().get_type(), TokenType::G_EQUALS);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<stringpool_t>(TokenType::MATH_SUB, stringPool));
    tokenizer.process_math_tokens(tokens);
    assertEqual(tokens.front().get_type(), TokenType::UNARY_NEG);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<stringpool_t>(TokenType::MATH_ADD, stringPool));
    tokens.push_back(Token<stringpool_t>(TokenType::MATH_SUB, stringPool));
    tokenizer.process_math_tokens(tokens);
    tokens.pop_front();
    assertEqual(tokens.front().get_type(), TokenType::UNARY_NEG);
    tokens.pop_front();
    assertEqual(tokens.size(), 0);

    tokens.push_back(Token<stringpool_t>(TokenType::OP_PAREN, stringPool));
    tokens.push_back(Token<stringpool_t>(TokenType::MATH_SUB, stringPool));
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
    Tokenizer<alloc_t, stringpool_t> tokenizer(alloc, stringPool);
    Tokenizer<alloc_t, stringpool_t> tokenizer2(alloc, stringPool, PoolString<stringpool_t>(stringPool));

    Test::min_verbosity = prevTestVerbosity;
}

test(tokenizer_unknown_token)
{
    int prevTestVerbosity = Test::min_verbosity;
    PoolString<stringpool_t> testName(stringPool, "tokenizer_unknown_token");    
    
    Serial.println("Test tokenizer_unknown_token starting.");
    typedef Token<stringpool_t> token_t;
    Deque<token_t, alloc_t> expectedTokens(alloc);
    Deque<token_t, alloc_t> generatedTokens(alloc);
    PoolString<stringpool_t> command(stringPool);

    command = "Blah";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());
    
    command = "$#";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
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
    PoolString<stringpool_t> testName(stringPool, "tokenizer_tokenize_missing_function_arguments");    

    Serial.println("Test tokenizer_tokenize_missing_function_arguments starting.");
    typedef Token<stringpool_t> token_t;
    Deque<token_t, alloc_t> expectedTokens(alloc);
    Deque<token_t, alloc_t> generatedTokens(alloc);
    PoolString<stringpool_t> command(stringPool);

    command = "answer IsNumber()";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "answer"));
    expectedTokens.push_back(token_t(TokenType::CREATE_NUM, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "0"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "answer IsNumber(42)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "answer"));
    expectedTokens.push_back(token_t(TokenType::CREATE_NUM, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "42"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED()";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::CREATE_LED, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "13"));
    expectedTokens.push_back(token_t(TokenType::COMMA, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "50"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED(15)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::CREATE_LED, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "15"));
    expectedTokens.push_back(token_t(TokenType::COMMA, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "50"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED(15, 25)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::CREATE_LED, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "15"));
    expectedTokens.push_back(token_t(TokenType::COMMA, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "25"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup()";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "blink"));
    expectedTokens.push_back(token_t(TokenType::RUN_GROUP, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "1"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup(-1)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "blink"));
    expectedTokens.push_back(token_t(TokenType::RUN_GROUP, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::UNARY_NEG, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "1"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup(10)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "blink"));
    expectedTokens.push_back(token_t(TokenType::RUN_GROUP, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "10"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
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
    PoolString<stringpool_t> testName(stringPool, "tokenizer_tokenize_functions");    

    Serial.println("Test tokenizer_tokenize_functions starting.");
    typedef Token<stringpool_t> token_t;
    Deque<token_t, alloc_t> expectedTokens(alloc);
    Deque<token_t, alloc_t> generatedTokens(alloc);
    PoolString<stringpool_t> command(stringPool);

    command = "answer IsNumber(42)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "answer"));
    expectedTokens.push_back(token_t(TokenType::CREATE_NUM, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "42"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light IsLED(13, 25)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::CREATE_LED, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "13"));
    expectedTokens.push_back(token_t(TokenType::COMMA, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "25"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink IsGroup (";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "blink"));
    expectedTokens.push_back(token_t(TokenType::CREATE_GROUP, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "blink RunGroup(10)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "blink"));
    expectedTokens.push_back(token_t(TokenType::RUN_GROUP, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "10"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light MoveByFor(100, 1000)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::MOVE_BY_FOR, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "100"));
    expectedTokens.push_back(token_t(TokenType::COMMA, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "1000"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light MoveBy(100)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::MOVE_BY, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "100"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light SetToFor(100, 1000)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::SET_TO_FOR, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "100"));
    expectedTokens.push_back(token_t(TokenType::COMMA, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "1000"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "light SetTo(100)";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "light"));
    expectedTokens.push_back(token_t(TokenType::SET_TO, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "100"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "If (answer < 100) (";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::IF, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::NAME, stringPool, "answer"));
    expectedTokens.push_back(token_t(TokenType::LESS, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "100"));
    expectedTokens.push_back(token_t(TokenType::CL_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "Else (";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::ELSE, stringPool));
    expectedTokens.push_back(token_t(TokenType::OP_PAREN, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
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
    PoolString<stringpool_t> testName(stringPool, "tokenizer_tokenize_string");    

    Serial.println("Test tokenizer_tokenize_string starting.");
    typedef Token<stringpool_t> token_t;
    Deque<token_t, alloc_t> expectedTokens(alloc);
    Deque<token_t, alloc_t> generatedTokens(alloc);
    PoolString<stringpool_t> command(stringPool);

    command = "'hello!'";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::STRING, stringPool, "hello!"));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    command = "\"hello!\"";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::STRING, stringPool, "hello!"));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
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
    PoolString<stringpool_t> testName(stringPool, "tokenizer_tokenize_math_expression");    

    Serial.println("Test tokenizer_tokenize_math_expression starting.");
    typedef Token<stringpool_t> token_t;
    Deque<token_t, alloc_t> expectedTokens(alloc);
    Deque<token_t, alloc_t> generatedTokens(alloc);
    PoolString<stringpool_t> command(stringPool);

    // 3 is to ensure we get sub and not unary neg
    command = "=<>+3-*/%^&|!~";
    expectedTokens.clear();
    expectedTokens.push_back(token_t(TokenType::EQUALS, stringPool));
    expectedTokens.push_back(token_t(TokenType::LESS, stringPool));
    expectedTokens.push_back(token_t(TokenType::GREATER, stringPool));
    expectedTokens.push_back(token_t(TokenType::MATH_ADD, stringPool));
    expectedTokens.push_back(token_t(TokenType::NUM_VAL, stringPool, "3"));
    expectedTokens.push_back(token_t(TokenType::MATH_SUB, stringPool));
    expectedTokens.push_back(token_t(TokenType::MATH_MUL, stringPool));
    expectedTokens.push_back(token_t(TokenType::MATH_DIV, stringPool));
    expectedTokens.push_back(token_t(TokenType::MATH_MOD, stringPool));
    expectedTokens.push_back(token_t(TokenType::MATH_POW, stringPool));
    expectedTokens.push_back(token_t(TokenType::LOGI_AND, stringPool));
    expectedTokens.push_back(token_t(TokenType::LOGI_OR, stringPool));
    expectedTokens.push_back(token_t(TokenType::LOGI_XOR, stringPool));
    expectedTokens.push_back(token_t(TokenType::LOGI_NOT, stringPool));
    expectedTokens.push_back(token_t(TokenType::CMD_END, stringPool));
    tokenizer.set_command(command);
    generatedTokens = tokenizer.tokenize();
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(set_command) [" + command + "]").c_str());
    generatedTokens.clear();
    generatedTokens = tokenizer.tokenize(command);
    tokenizer_check_tokens_match(generatedTokens, expectedTokens, (testName + "(tokenize) [" + command + "]").c_str());

    Test::min_verbosity = prevTestVerbosity;
}

void tokenizer_check_tokens_match(Deque<Token<stringpool_t>, alloc_t> & generatedTokens, 
                                  Deque<Token<stringpool_t>, alloc_t> & expectedTokens, 
                                  char const * comment) {
    assertEqual(generatedTokens.size(), expectedTokens.size(), comment);
    Deque<Token<stringpool_t>, alloc_t>::Iterator genIter = generatedTokens.begin();
    Deque<Token<stringpool_t>, alloc_t>::Iterator expIter = expectedTokens.begin();
    int i = 0;
    while (genIter != generatedTokens.end() && expIter != expectedTokens.end()) {
        assertEqual(genIter->get_type(), expIter->get_type(), "i = " << i << ": " << comment);
        assertEqual(genIter->get_value().c_str(), expIter->get_value().c_str(), "i = " << i << ": " << comment);
        ++genIter;
        ++expIter;
        ++i;
    }
}
