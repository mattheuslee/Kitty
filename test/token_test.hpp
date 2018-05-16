#pragma once

#include <cstring>

#include <kty/containers/string.hpp>
#include <kty/token.hpp>

using namespace kty;

test(token_constructor)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_constructor starting.");

    /** First constructor type */
    Token<> token1;
    assertEqual(token1.get_type(), TokenType::UNKNOWN_TOKEN);
    assertEqual(token1.get_value().c_str(), "");

    /** Second constructor type */
    Token<> token2(TokenType::CREATE_NUM);
    assertEqual(token2.get_type(), TokenType::CREATE_NUM);
    assertEqual(token2.get_value().c_str(), "");

    /** Third constructor type */
    PoolString<> value("answer");
    Token<> token3(TokenType::NAME, value);
    assertEqual(token3.get_type(), TokenType::NAME);
    assertEqual(token3.get_value().c_str(), "answer");

    value = "42";
    Token<> token4(TokenType::NUM_VAL, value);
    assertEqual(token4.get_type(), TokenType::NUM_VAL);
    assertEqual(token4.get_value().c_str(), "42");

    Test::min_verbosity = prevTestVerbosity;
}

test(token_getters_setters)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_getters_setters starting.");
    PoolString<> value;
    Token<> token;

    assertEqual(token.get_type(), TokenType::UNKNOWN_TOKEN);
    assertEqual(token.get_value().c_str(), "");

    token.set_type(TokenType::NAME);
    value = "answer";
    token.set_value(value);
    assertEqual(token.get_type(), TokenType::NAME);
    assertEqual(token.get_value().c_str(), "answer");

    token.set_type(TokenType::NUM_VAL);
    value = "42";
    token.set_value(value);
    assertEqual(token.get_type(), TokenType::NUM_VAL);
    assertEqual(token.get_value().c_str(), "42");

    Test::min_verbosity = prevTestVerbosity;
}

test(token_str)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_str starting.");
    PoolString<> value;

    /** Empty value string */
    Token<> token(TokenType::CREATE_NUM, value);
    assertEqual(token.str().c_str(), "Token(CREATE_NUM, )");

    /** With value string */
    value = "answer";
    token.set_type(TokenType::NAME);
    token.set_value(value);
    assertEqual(token.str().c_str(), "Token(NAME, answer)");
    value = "42";
    token.set_type(TokenType::NUM_VAL);
    token.set_value(value);
    assertEqual(token.str().c_str(), "Token(NUM_VAL, 42)");

    Test::min_verbosity = prevTestVerbosity;
}

test(token_type_as_c_str)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_type_as_c_str starting.");
    Token<> token;

    token.set_type(TokenType::CREATE_NUM);
    assertEqual(token.type_as_c_str(), "CREATE_NUM");
    token.set_type(TokenType::CREATE_LED);
    assertEqual(token.type_as_c_str(), "CREATE_LED");
    token.set_type(TokenType::CREATE_GROUP);
    assertEqual(token.type_as_c_str(), "CREATE_GROUP");
    token.set_type(TokenType::RUN_GROUP);
    assertEqual(token.type_as_c_str(), "RUN_GROUP");
    token.set_type(TokenType::MOVE_BY_FOR);
    assertEqual(token.type_as_c_str(), "MOVE_BY_FOR");
    token.set_type(TokenType::MOVE_BY);
    assertEqual(token.type_as_c_str(), "MOVE_BY");
    token.set_type(TokenType::SET_TO_FOR);
    assertEqual(token.type_as_c_str(), "SET_TO_FOR");
    token.set_type(TokenType::SET_TO);
    assertEqual(token.type_as_c_str(), "SET_TO");
    token.set_type(TokenType::PRINT);
    assertEqual(token.type_as_c_str(), "PRINT");
    token.set_type(TokenType::NAME);
    assertEqual(token.type_as_c_str(), "NAME");
    token.set_type(TokenType::NUM_VAL);
    assertEqual(token.type_as_c_str(), "NUM_VAL");
    token.set_type(TokenType::STRING);
    assertEqual(token.type_as_c_str(), "STRING");
    token.set_type(TokenType::IF);
    assertEqual(token.type_as_c_str(), "IF");
    token.set_type(TokenType::ELSE);
    assertEqual(token.type_as_c_str(), "ELSE");
    token.set_type(TokenType::OP_PAREN);
    assertEqual(token.type_as_c_str(), "OP_PAREN");
    token.set_type(TokenType::CL_PAREN);
    assertEqual(token.type_as_c_str(), "CL_PAREN");
    token.set_type(TokenType::COMMA);
    assertEqual(token.type_as_c_str(), "COMMA");
    token.set_type(TokenType::EQUALS);
    assertEqual(token.type_as_c_str(), "EQUALS");
    token.set_type(TokenType::L_EQUALS);
    assertEqual(token.type_as_c_str(), "L_EQUALS");
    token.set_type(TokenType::G_EQUALS);
    assertEqual(token.type_as_c_str(), "G_EQUALS");
    token.set_type(TokenType::LESS);
    assertEqual(token.type_as_c_str(), "LESS");
    token.set_type(TokenType::GREATER);
    assertEqual(token.type_as_c_str(), "GREATER");
    token.set_type(TokenType::MATH_ADD);
    assertEqual(token.type_as_c_str(), "MATH_ADD");
    token.set_type(TokenType::MATH_SUB);
    assertEqual(token.type_as_c_str(), "MATH_SUB");
    token.set_type(TokenType::MATH_MUL);
    assertEqual(token.type_as_c_str(), "MATH_MUL");
    token.set_type(TokenType::MATH_DIV);
    assertEqual(token.type_as_c_str(), "MATH_DIV");
    token.set_type(TokenType::MATH_MOD);
    assertEqual(token.type_as_c_str(), "MATH_MOD");
    token.set_type(TokenType::MATH_POW);
    assertEqual(token.type_as_c_str(), "MATH_POW");
    token.set_type(TokenType::UNARY_NEG);
    assertEqual(token.type_as_c_str(), "UNARY_NEG");
    token.set_type(TokenType::LOGI_AND);
    assertEqual(token.type_as_c_str(), "LOGI_AND");
    token.set_type(TokenType::LOGI_OR);
    assertEqual(token.type_as_c_str(), "LOGI_OR");
    token.set_type(TokenType::LOGI_XOR);
    assertEqual(token.type_as_c_str(), "LOGI_XOR");
    token.set_type(TokenType::LOGI_NOT);
    assertEqual(token.type_as_c_str(), "LOGI_NOT");
    token.set_type(TokenType::CMD_END);
    assertEqual(token.type_as_c_str(), "CMD_END");
    token.set_type(TokenType::UNKNOWN_TOKEN);
    assertEqual(token.type_as_c_str(), "UNKNOWN_TOKEN");

    Test::min_verbosity = prevTestVerbosity;
}

test(token_precedence_level)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_precedence_level starting.");
    Token<> token;

    token.set_type(TokenType::CREATE_NUM);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::CREATE_LED);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::CREATE_GROUP);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::RUN_GROUP);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::MOVE_BY_FOR);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::MOVE_BY);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::SET_TO_FOR);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::SET_TO);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::PRINT);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::NAME);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::NUM_VAL);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::STRING);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::IF);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::ELSE);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::OP_PAREN);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::CL_PAREN);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::COMMA);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::EQUALS);
    assertEqual(token.precedence_level(), 2, token.str().c_str());
    token.set_type(TokenType::L_EQUALS);
    assertEqual(token.precedence_level(), 2, token.str().c_str());
    token.set_type(TokenType::G_EQUALS);
    assertEqual(token.precedence_level(), 2, token.str().c_str());
    token.set_type(TokenType::LESS);
    assertEqual(token.precedence_level(), 2, token.str().c_str());
    token.set_type(TokenType::GREATER);
    assertEqual(token.precedence_level(), 2, token.str().c_str());
    token.set_type(TokenType::MATH_ADD);
    assertEqual(token.precedence_level(), 3, token.str().c_str());
    token.set_type(TokenType::MATH_SUB);
    assertEqual(token.precedence_level(), 3, token.str().c_str());
    token.set_type(TokenType::MATH_MUL);
    assertEqual(token.precedence_level(), 4, token.str().c_str());
    token.set_type(TokenType::MATH_DIV);
    assertEqual(token.precedence_level(), 4, token.str().c_str());
    token.set_type(TokenType::MATH_MOD);
    assertEqual(token.precedence_level(), 4, token.str().c_str());
    token.set_type(TokenType::MATH_POW);
    assertEqual(token.precedence_level(), 5, token.str().c_str());
    token.set_type(TokenType::UNARY_NEG);
    assertEqual(token.precedence_level(), 6, token.str().c_str());
    token.set_type(TokenType::LOGI_AND);
    assertEqual(token.precedence_level(), 1, token.str().c_str());
    token.set_type(TokenType::LOGI_OR);
    assertEqual(token.precedence_level(), 1, token.str().c_str());
    token.set_type(TokenType::LOGI_XOR);
    assertEqual(token.precedence_level(), 1, token.str().c_str());
    token.set_type(TokenType::LOGI_NOT);
    assertEqual(token.precedence_level(), 6, token.str().c_str());
    token.set_type(TokenType::CMD_END);
    assertEqual(token.precedence_level(), 0, token.str().c_str());
    token.set_type(TokenType::UNKNOWN_TOKEN);
    assertEqual(token.precedence_level(), 0, token.str().c_str());

    Test::min_verbosity = prevTestVerbosity;    
}

test(token_num_function_arguments)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_num_function_arguments starting.");
    Token<> token;

    token.set_type(TokenType::CREATE_NUM);
    assertEqual(token.num_function_arguments(), 1, token.str().c_str());
    token.set_type(TokenType::CREATE_LED);
    assertEqual(token.num_function_arguments(), 2, token.str().c_str());
    token.set_type(TokenType::CREATE_GROUP);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::RUN_GROUP);
    assertEqual(token.num_function_arguments(), 1, token.str().c_str());
    token.set_type(TokenType::MOVE_BY_FOR);
    assertEqual(token.num_function_arguments(), 2, token.str().c_str());
    token.set_type(TokenType::MOVE_BY);
    assertEqual(token.num_function_arguments(), 1, token.str().c_str());
    token.set_type(TokenType::SET_TO_FOR);
    assertEqual(token.num_function_arguments(), 2, token.str().c_str());
    token.set_type(TokenType::SET_TO);
    assertEqual(token.num_function_arguments(), 1, token.str().c_str());
    token.set_type(TokenType::PRINT);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::NAME);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::NUM_VAL);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::STRING);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::IF);
    assertEqual(token.num_function_arguments(), 1, token.str().c_str());
    token.set_type(TokenType::ELSE);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::OP_PAREN);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::CL_PAREN);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::COMMA);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::EQUALS);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::L_EQUALS);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::G_EQUALS);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::LESS);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::GREATER);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::MATH_ADD);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::MATH_SUB);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::MATH_MUL);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::MATH_DIV);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::MATH_MOD);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::MATH_POW);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::UNARY_NEG);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::LOGI_AND);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::LOGI_OR);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::LOGI_XOR);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::LOGI_NOT);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::CMD_END);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());
    token.set_type(TokenType::UNKNOWN_TOKEN);
    assertEqual(token.num_function_arguments(), 0, token.str().c_str());

    Test::min_verbosity = prevTestVerbosity;
}

test(token_precedence_comparisons)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_precedence_comparisons starting.");
    Token<> token1;
    Token<> token2;

    /** Level 6 vs Level 6 */
    token1.set_type(TokenType::UNARY_NEG);
    token2.set_type(TokenType::LOGI_NOT);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 6 vs Level 5 */
    token2.set_type(TokenType::MATH_POW);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 6 vs Level 4 */
    token2.set_type(TokenType::MATH_MUL);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());    
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());    

    /** Level 6 vs Level 3 */
    token2.set_type(TokenType::MATH_ADD);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());    
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());    

    /** Level 6 vs Level 2 */
    token2.set_type(TokenType::EQUALS);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());    
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());    

    /** Level 6 vs Level 1 */
    token2.set_type(TokenType::LOGI_AND);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());    
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());    

    /** Level 6 vs Level 0 */
    token2.set_type(TokenType::CREATE_NUM);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());    
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());    

    /** Level 5 vs Level 5 */
    token1.set_type(TokenType::MATH_POW);
    token2.set_type(TokenType::MATH_POW);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 5 vs Level 4 */
    token2.set_type(TokenType::MATH_MUL);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());
    
    /** Level 5 vs Level 3 */
    token2.set_type(TokenType::MATH_ADD);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 5 vs Level 2 */
    token2.set_type(TokenType::EQUALS);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 5 vs Level 1 */
    token2.set_type(TokenType::LOGI_AND);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 5 vs Level 0 */
    token2.set_type(TokenType::CREATE_NUM);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 4 vs Level 4 */
    token1.set_type(TokenType::MATH_MUL);
    token2.set_type(TokenType::MATH_DIV);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 4 vs Level 3 */
    token2.set_type(TokenType::MATH_ADD);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 4 vs Level 2 */
    token2.set_type(TokenType::EQUALS);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 4 vs Level 1 */
    token2.set_type(TokenType::LOGI_AND);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 4 vs Level 0 */
    token2.set_type(TokenType::CREATE_NUM);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 3 vs Level 3 */
    token1.set_type(TokenType::MATH_ADD);
    token2.set_type(TokenType::MATH_SUB);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 3 vs Level 2 */
    token2.set_type(TokenType::EQUALS);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 3 vs Level 1 */
    token2.set_type(TokenType::LOGI_AND);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 3 vs Level 0 */
    token2.set_type(TokenType::CREATE_NUM);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 2 vs Level 2 */
    token1.set_type(TokenType::EQUALS);
    token2.set_type(TokenType::L_EQUALS);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 2 vs Level 1 */
    token2.set_type(TokenType::LOGI_AND);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 2 vs Level 0 */
    token2.set_type(TokenType::CREATE_NUM);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 1 vs Level 1 */
    token1.set_type(TokenType::LOGI_AND);
    token2.set_type(TokenType::LOGI_OR);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 1 vs Level 0 */
    token2.set_type(TokenType::CREATE_NUM);
    assertTrue(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertFalse(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertFalse(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    /** Level 0 vs Level 0 */
    token1.set_type(TokenType::CREATE_NUM);
    token2.set_type(TokenType::CREATE_LED);
    assertFalse(token1.has_greater_precedence_than(token2), token1.str().c_str() << " >p " << token2.str().c_str());
    assertFalse(token2.has_greater_precedence_than(token1), token1.str().c_str() << " >p " << token2.str().c_str());    
    assertTrue(token1.has_equal_precedence_to(token2), token1.str().c_str() << " ==p " << token2.str().c_str());
    assertTrue(token2.has_equal_precedence_to(token1), token1.str().c_str() << " ==p " << token2.str().c_str());

    Test::min_verbosity = prevTestVerbosity;
}

test(token_type_checkers)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_type_checkers starting.");
    Token<> token;

    token.set_type(TokenType::CREATE_NUM);
    assertTrue(token.is_create_num(), token.str().c_str());
    assertTrue(token.is_create_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::CREATE_LED);
    assertTrue(token.is_create_led(), token.str().c_str());
    assertTrue(token.is_create_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::CREATE_GROUP);
    assertTrue(token.is_create_group(), token.str().c_str());
    assertTrue(token.is_create_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::RUN_GROUP);
    assertTrue(token.is_run_group(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::MOVE_BY_FOR);
    assertTrue(token.is_move_by_for(), token.str().c_str());
    assertTrue(token.is_move_by_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::MOVE_BY);
    assertTrue(token.is_move_by(), token.str().c_str());
    assertTrue(token.is_move_by_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::SET_TO_FOR);
    assertTrue(token.is_set_to_for(), token.str().c_str());
    assertTrue(token.is_set_to_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::SET_TO);
    assertTrue(token.is_set_to(), token.str().c_str());
    assertTrue(token.is_set_to_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::PRINT);
    assertTrue(token.is_print(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());
    token.set_type(TokenType::NAME);
    assertTrue(token.is_name(), token.str().c_str());
    assertTrue(token.is_operand(), token.str().c_str());
    token.set_type(TokenType::NUM_VAL);
    assertTrue(token.is_num_val(), token.str().c_str());
    assertTrue(token.is_operand(), token.str().c_str());
    token.set_type(TokenType::STRING);
    assertTrue(token.is_string(), token.str().c_str());
    token.set_type(TokenType::IF);
    assertTrue(token.is_if(), token.str().c_str());
    assertTrue(token.is_conditional_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());    
    token.set_type(TokenType::ELSE);
    assertTrue(token.is_else(), token.str().c_str());
    assertTrue(token.is_conditional_command(), token.str().c_str());
    assertTrue(token.is_function(), token.str().c_str());    
    token.set_type(TokenType::OP_PAREN);
    assertTrue(token.is_op_paren(), token.str().c_str());
    token.set_type(TokenType::CL_PAREN);
    assertTrue(token.is_cl_paren(), token.str().c_str());
    token.set_type(TokenType::COMMA);
    assertTrue(token.is_comma(), token.str().c_str());
    token.set_type(TokenType::EQUALS);
    assertTrue(token.is_equals(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::L_EQUALS);
    assertTrue(token.is_l_equals(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::G_EQUALS);
    assertTrue(token.is_g_equals(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::LESS);
    assertTrue(token.is_less(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::GREATER);
    assertTrue(token.is_greater(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::MATH_ADD);
    assertTrue(token.is_math_add(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::MATH_SUB);
    assertTrue(token.is_math_sub(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::MATH_MUL);
    assertTrue(token.is_math_mul(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::MATH_DIV);
    assertTrue(token.is_math_div(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::MATH_MOD);
    assertTrue(token.is_math_mod(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::MATH_POW);
    assertTrue(token.is_math_pow(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertFalse(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::UNARY_NEG);
    assertTrue(token.is_unary_neg(), token.str().c_str());
    assertFalse(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    token.set_type(TokenType::LOGI_AND);
    assertTrue(token.is_logi_and(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::LOGI_OR);
    assertTrue(token.is_logi_or(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());    
    token.set_type(TokenType::LOGI_XOR);
    assertTrue(token.is_logi_xor(), token.str().c_str());
    assertTrue(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    assertTrue(token.is_left_associative(), token.str().c_str());
    token.set_type(TokenType::LOGI_NOT);
    assertTrue(token.is_logi_not(), token.str().c_str());
    assertFalse(token.is_binary_operator(), token.str().c_str());
    assertTrue(token.is_operator(), token.str().c_str());
    token.set_type(TokenType::CMD_END);
    assertTrue(token.is_cmd_end(), token.str().c_str());
    token.set_type(TokenType::UNKNOWN_TOKEN);
    assertTrue(token.is_unknown_token(), token.str().c_str());

    Test::min_verbosity = prevTestVerbosity;
}

test(token_command_str_to_token_type)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_command_str_to_token_type starting.");
    PoolString<> str;

    str = "IsNumber";
    assertEqual(command_str_to_token_type(str), TokenType::CREATE_NUM, str.c_str());
    assertEqual(command_str_to_token_type("IsNumber"), TokenType::CREATE_NUM);
    str = "IsLED";
    assertEqual(command_str_to_token_type(str), TokenType::CREATE_LED, str.c_str());
    assertEqual(command_str_to_token_type("IsLED"), TokenType::CREATE_LED);
    str = "IsGroup";
    assertEqual(command_str_to_token_type(str), TokenType::CREATE_GROUP, str.c_str());
    assertEqual(command_str_to_token_type("IsGroup"), TokenType::CREATE_GROUP);
    str = "RunGroup";
    assertEqual(command_str_to_token_type(str), TokenType::RUN_GROUP, str.c_str());
    assertEqual(command_str_to_token_type("RunGroup"), TokenType::RUN_GROUP);
    str = "MoveByFor";
    assertEqual(command_str_to_token_type(str), TokenType::MOVE_BY_FOR, str.c_str());
    assertEqual(command_str_to_token_type("MoveByFor"), TokenType::MOVE_BY_FOR);
    str = "MoveBy";
    assertEqual(command_str_to_token_type(str), TokenType::MOVE_BY, str.c_str());
    assertEqual(command_str_to_token_type("MoveBy"), TokenType::MOVE_BY);
    str = "SetToFor";
    assertEqual(command_str_to_token_type(str), TokenType::SET_TO_FOR, str.c_str());
    assertEqual(command_str_to_token_type("SetToFor"), TokenType::SET_TO_FOR);
    str = "SetTo";
    assertEqual(command_str_to_token_type(str), TokenType::SET_TO, str.c_str());
    assertEqual(command_str_to_token_type("SetTo"), TokenType::SET_TO);
    str = "Print";
    assertEqual(command_str_to_token_type(str), TokenType::PRINT, str.c_str());
    assertEqual(command_str_to_token_type("Print"), TokenType::PRINT);
    str = "If";
    assertEqual(command_str_to_token_type(str), TokenType::IF, str.c_str());
    assertEqual(command_str_to_token_type("If"), TokenType::IF);
    str = "Else";
    assertEqual(command_str_to_token_type(str), TokenType::ELSE, str.c_str());
    assertEqual(command_str_to_token_type("Else"), TokenType::ELSE);
    str = "";
    assertEqual(command_str_to_token_type(str), TokenType::UNKNOWN_TOKEN, str.c_str());
    assertEqual(command_str_to_token_type(""), TokenType::UNKNOWN_TOKEN);
    str = "Invalid";
    assertEqual(command_str_to_token_type(str), TokenType::UNKNOWN_TOKEN, str.c_str());
    assertEqual(command_str_to_token_type("Invalid"), TokenType::UNKNOWN_TOKEN);

    Test::min_verbosity = prevTestVerbosity;
}

test(token_punctuation_char_to_token_type)
{
    int prevTestVerbosity = Test::min_verbosity;

    Serial.println("Test token_punctuation_char_to_token_type starting."); 

    char c;
    c = '(';
    assertEqual(punctuation_char_to_token_type(c), TokenType::OP_PAREN, c);
    c = ')';
    assertEqual(punctuation_char_to_token_type(c), TokenType::CL_PAREN, c);
    c = ',';
    assertEqual(punctuation_char_to_token_type(c), TokenType::COMMA, c);
    c = '=';
    assertEqual(punctuation_char_to_token_type(c), TokenType::EQUALS, c);
    c = '<';
    assertEqual(punctuation_char_to_token_type(c), TokenType::LESS, c);
    c = '>';
    assertEqual(punctuation_char_to_token_type(c), TokenType::GREATER, c);
    c = '+';
    assertEqual(punctuation_char_to_token_type(c), TokenType::MATH_ADD, c);
    c = '-';
    assertEqual(punctuation_char_to_token_type(c), TokenType::MATH_SUB, c);
    c = '*';
    assertEqual(punctuation_char_to_token_type(c), TokenType::MATH_MUL, c);
    c = '/';
    assertEqual(punctuation_char_to_token_type(c), TokenType::MATH_DIV, c);
    c = '%';
    assertEqual(punctuation_char_to_token_type(c), TokenType::MATH_MOD, c);
    c = '^';
    assertEqual(punctuation_char_to_token_type(c), TokenType::MATH_POW, c);
    c = '&';
    assertEqual(punctuation_char_to_token_type(c), TokenType::LOGI_AND, c);
    c = '|';
    assertEqual(punctuation_char_to_token_type(c), TokenType::LOGI_OR, c);
    c = '!';
    assertEqual(punctuation_char_to_token_type(c), TokenType::LOGI_XOR, c);
    c = '~';
    assertEqual(punctuation_char_to_token_type(c), TokenType::LOGI_NOT, c);
    c = ' ';
    assertEqual(punctuation_char_to_token_type(c), TokenType::UNKNOWN_TOKEN, c);
    c = '#';
    assertEqual(punctuation_char_to_token_type(c), TokenType::UNKNOWN_TOKEN, c);

    Test::min_verbosity = prevTestVerbosity;
}

