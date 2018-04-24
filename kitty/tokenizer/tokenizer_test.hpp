#pragma once

#include <kitty/stl_impl.hpp>
#include <string>

#include <kitty/tokenizer/tokenizer.hpp>

using namespace std;
using namespace kitty;

test(token_constructor)
{
    Token token(TokenType::CREATE_NUM);
    assertTrue(token.type == TokenType::CREATE_NUM);
    assertTrue(token.value == "");

    token = Token(TokenType::CREATE_LED);
    assertTrue(token.type == TokenType::CREATE_LED);
    assertTrue(token.value == "");

    token = Token(TokenType::CREATE_SERVO);
    assertTrue(token.type == TokenType::CREATE_SERVO);
    assertTrue(token.value == "");
    
    token = Token(TokenType::VAR_NAME, "answer");
    assertTrue(token.type == TokenType::VAR_NAME);
    assertTrue(token.value == "answer");

    token = Token(TokenType::NUM_VAL, "5");
    assertTrue(token.type == TokenType::NUM_VAL);
    assertTrue(token.value == "5");

    token = Token(TokenType::UNKNOWN);
    assertTrue(token.type == TokenType::UNKNOWN);
    assertTrue(token.value == "");
}

test(token_str)
{
    Token token(TokenType::CREATE_NUM);
    assertTrue(token.str() == "Token(CREATE_NUM, )");

    token = Token(TokenType::CREATE_LED);
    assertTrue(token.str() == "Token(CREATE_LED, )");

    token = Token(TokenType::CREATE_SERVO);
    assertTrue(token.str() == "Token(CREATE_SERVO, )");
    
    token = Token(TokenType::VAR_NAME, "answer");
    assertTrue(token.str() == "Token(VAR_NAME, answer)");

    token = Token(TokenType::NUM_VAL, "42");
    assertTrue(token.str() == "Token(NUM_VAL, 42)");
    
    token = Token(TokenType::UNKNOWN);
    assertTrue(token.str() == "Token(UNKNOWN, )");
}

test(tokenizer_get_next_token) 
{
    Tokenizer tokenizer("CreateNumber CreateLED CreateServo answer 42 Rubbish");
    TokenType expectedTypes[] = {TokenType::CREATE_NUM, 
                                 TokenType::CREATE_LED, 
                                 TokenType::CREATE_SERVO,
                                 TokenType::VAR_NAME, 
                                 TokenType::NUM_VAL, 
                                 TokenType::UNKNOWN, 
                                 TokenType::CMD_END};
    std::string expectedValues[] = {"", "", "", "answer", "42", "", ""};
    int expectedNumTokens = 7;

    for (int i = 0; i < expectedNumTokens; ++i) {
        Token token = tokenizer.get_next_token();
        assertTrue(token.type == expectedTypes[i], "i = " << i);
        assertTrue(token.value == expectedValues[i], "i = " << i);
    }
}
