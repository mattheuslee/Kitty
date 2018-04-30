#pragma once

#include <kty/stl_impl.hpp>
#include <string>
#include <vector>

#include <kty/tokenizer.hpp>

using namespace std;
using namespace kty;

test(tokenizer_token_constructor)
{
    string value;
    Token token(TokenType::CREATE_NUM);
    assertTrue(token.type == TokenType::CREATE_NUM);
    assertTrue(token.value == "");
    
    value = "answer";
    token = Token(TokenType::NAME, value);
    assertTrue(token.type == TokenType::NAME);
    assertTrue(token.value == "answer");

    value = "42";
    token = Token(TokenType::NUM_VAL, value);
    assertTrue(token.type == TokenType::NUM_VAL);
    assertTrue(token.value == "42");
}

test(tokenizer_token_str)
{
    string value;
    Token token(TokenType::CREATE_NUM);
    assertTrue(token.str() == "Token(CREATE_NUM, )");   
    
    value = "answer";
    token = Token(TokenType::NAME, value);
    assertTrue(token.str() == "Token(NAME, answer)");

    value = "42";
    token = Token(TokenType::NUM_VAL, value);
    assertTrue(token.str() == "Token(NUM_VAL, 42)");
}

void tokenizer_check_correct_types_values(Tokenizer & tokenizer, vector<TokenType> & expectedTypes, vector<string> & expectedValues, string comment) {
    //Serial.println("");
    for (int i = 0; i < expectedTypes.size(); ++i) {
        Token token = tokenizer.get_next_token();
        //Serial.println(token.str().c_str());
        assertTrue(token.type == expectedTypes[i], comment.c_str() << ": i = " << i);
        assertTrue(token.value == expectedValues[i], comment.c_str() << ": i = " << i);
    }
}

test(tokenizer_complete_commands)
{
    string command;
    Tokenizer tokenizer(command);
    vector<TokenType> expectedTypes;
    vector<string> expectedValues;

    command = "answer IsNumber()";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::CREATE_NUM, 
        TokenType::OP_PAREN,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "answer",
        "",
        "",
        "0",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command);

    command = "answer IsNumber(42)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::CREATE_NUM, 
        TokenType::OP_PAREN,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "answer",
        "",
        "",
        "42",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command);

    command = "light IsLED(13)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::CREATE_LED, 
        TokenType::OP_PAREN,
        TokenType::NUM_VAL,
        TokenType::COMMA,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "light",
        "",
        "",
        "13",
        "",
        "50",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command);

    command = "light IsLED(13, 25)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::CREATE_LED, 
        TokenType::OP_PAREN,
        TokenType::NUM_VAL,
        TokenType::COMMA,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "light",
        "",
        "",
        "13",
        "",
        "25",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command);

    command = "sweeper IsServo(10)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::CREATE_SERVO, 
        TokenType::OP_PAREN,
        TokenType::NUM_VAL,
        TokenType::COMMA,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "sweeper",
        "",
        "",
        "10",
        "",
        "90",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command);

    command = "sweeper IsServo(10, 45)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::CREATE_SERVO, 
        TokenType::OP_PAREN,
        TokenType::NUM_VAL,
        TokenType::COMMA,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "sweeper",
        "",
        "",
        "10",
        "",
        "45",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command);
    
    command = "If (num % 3 = 0 & num % 5 = 0) (";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::IF, 
        TokenType::OP_PAREN, 
        TokenType::NAME,
        TokenType::MATH_MOD,
        TokenType::NUM_VAL,
        TokenType::EQUALS,
        TokenType::NUM_VAL,
        TokenType::LOGI_AND, 
        TokenType::NAME,
        TokenType::MATH_MOD,
        TokenType::NUM_VAL,
        TokenType::EQUALS,
        TokenType::NUM_VAL,
        TokenType::CL_PAREN,
        TokenType::OP_PAREN, 
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "",
        "",
        "num",
        "",
        "3",
        "",
        "0",
        "",
        "num",
        "",
        "5",
        "",
        "0",
        "",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

    command = "Else (";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::ELSE, 
        TokenType::OP_PAREN, 
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

    command = "While (42 <= answer + 5) (";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::WHILE, 
        TokenType::OP_PAREN, 
        TokenType::NUM_VAL, 
        TokenType::LESS, 
        TokenType::EQUALS, 
        TokenType::NAME, 
        TokenType::MATH_ADD, 
        TokenType::NUM_VAL, 
        TokenType::CL_PAREN, 
        TokenType::OP_PAREN,         
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "",
        "",
        "42",
        "",
        "",
        "answer",
        "",
        "5",
        "",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

    command = "answer MoveBy(-10)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::MOVE_BY, 
        TokenType::OP_PAREN, 
        TokenType::MATH_SUB, 
        TokenType::NUM_VAL, 
        TokenType::CL_PAREN,         
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "answer",
        "",
        "",
        "",
        "10",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

    command = "light SetTo(75)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::SET_TO, 
        TokenType::OP_PAREN, 
        TokenType::NUM_VAL, 
        TokenType::CL_PAREN,         
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "light",
        "",
        "",
        "75",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

    command = "answer MoveByFor(-10, 1)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::MOVE_BY_FOR, 
        TokenType::OP_PAREN, 
        TokenType::MATH_SUB, 
        TokenType::NUM_VAL, 
        TokenType::COMMA, 
        TokenType::NUM_VAL, 
        TokenType::CL_PAREN,         
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "answer",
        "",
        "",
        "",
        "10",
        "",
        "1",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

    command = "light SetToFor(75, 1)";
    tokenizer.set_command(command);
    expectedTypes = vector<TokenType>({
        TokenType::NAME, 
        TokenType::SET_TO_FOR, 
        TokenType::OP_PAREN, 
        TokenType::NUM_VAL, 
        TokenType::COMMA, 
        TokenType::NUM_VAL, 
        TokenType::CL_PAREN,         
        TokenType::CMD_END
    });
    expectedValues = vector<string>({
        "light",
        "",
        "",
        "75",
        "",
        "1",
        "",
        ""
    });
    tokenizer_check_correct_types_values(tokenizer, expectedTypes, expectedValues, command); 

}
