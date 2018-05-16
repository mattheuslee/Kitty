#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/types.hpp>

namespace kty {

// Not using enum class due to int conversion requirement for ArduinoUnit
/** The various types of tokens possible */
enum TokenType {
    CREATE_NUM = 0, CREATE_LED, CREATE_GROUP, RUN_GROUP,
    MOVE_BY_FOR, MOVE_BY, SET_TO_FOR, SET_TO,
    NAME, NUM_VAL, STRING,
    IF, ELSE, 
    OP_PAREN, CL_PAREN, COMMA,
    EQUALS, L_EQUALS, G_EQUALS, LESS, GREATER,
    MATH_ADD, MATH_SUB, MATH_MUL, MATH_DIV, MATH_MOD, MATH_POW,
    UNARY_NEG,
    LOGI_AND, LOGI_OR, LOGI_XOR, LOGI_NOT,
    CMD_END,
    UNKNOWN_TOKEN,
};

/*!
    @brief  Class that contains all the information about a token.
*/
template <typename GetPoolFunc = decltype(get_stringpool)>
class Token {

public:
    /*!
        @brief  The constructor for a token.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    explicit Token(GetPoolFunc & getPoolFunc = get_stringpool)
        : value_(getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        type_ = TokenType::UNKNOWN_TOKEN;
    }

    /*!
        @brief  The constructor for a token.

        @param  type
                The type of token.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    Token(TokenType type, GetPoolFunc & getPoolFunc = get_stringpool) 
        : value_(getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        type_ = type;
    }

    /*!
        @brief  The constructor for a token.

        @param  type
                The type of token.
        
        @param  value
                The value to store in the token.
    */
    Token(TokenType type, PoolString<> const & value) 
        : value_(value) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        type_ = type;
        value_ = value;
    }

    /*!
        @brief  The constructor for the token.

        @param  type
                The type of token.
        
        @param  value
                The value to store in the token.
        
        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    Token(TokenType type, char const * value, GetPoolFunc & getPoolFunc = get_stringpool) 
        : value_(getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        type_ = type;
        value_ = value;
    }

    /*!
        @brief  Sets the type of the token.

        @param  type
                The type to set to.
    */
    void set_type(TokenType type) {
        Log.verbose(F("%s: setting to %d\n"), PRINT_FUNC, type);
        type_ = type;
    }

    /*!
        @brief  Gets the type of the token.

        @return The type of the token.
    */
    TokenType get_type() const {
        Log.verbose(F("%s: getting %d\n"), PRINT_FUNC, type_);
        return type_;
    }

    /*!
        @brief  Sets the value of the token.

        @param  value
                The value to set to.
    */
    void set_value(PoolString<> const & value) {
        Log.verbose(F("%s: setting to %s\n"), PRINT_FUNC, value.c_str());
        value_ = value;
    }

    /*!
        @brief  Gets the value of the token.

        @return A copy of the value of the token.
    */
    PoolString<> get_value() const {
        Log.verbose(F("%s: getting %s\n"), PRINT_FUNC, value_.c_str());
        return value_;
    }

    /*!
        @brief  Gets the string reprerentation of the token for debugging.

        @return The string representaion of the token.
    */
    PoolString<> str() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Copy assignment so we don't need to store the pool
        PoolString<> result(value_);
        result = "Token(";
        result += type_as_c_str();
        result += ", ";
        result += value_;
        result += ")";
        Log.verbose(F("%s: result %s\n"), PRINT_FUNC, result.c_str());
        return result;
    }

    /*!
        @brief  Gets the string representation of the type of the token.

        @return The string representation of the type of the token.
    */
    char const * type_as_c_str() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        static const char lookup[][14] = {
            "CREATE_NUM",
            "CREATE_LED",
            "CREATE_GROUP",
            "RUN_GROUP",
            "MOVE_BY_FOR",
            "MOVE_BY",
            "SET_TO_FOR",
            "SET_TO",
            "NAME",
            "NUM_VAL",
            "STRING",
            "IF",
            "ELSE",
            "OP_PAREN",
            "CL_PAREN",
            "COMMA",
            "EQUALS",
            "L_EQUALS",
            "G_EQUALS",
            "LESS",
            "GREATER",
            "MATH_ADD",
            "MATH_SUB",
            "MATH_MUL",
            "MATH_DIV",
            "MATH_MOD",
            "MATH_POW",
            "UNARY_NEG",
            "LOGI_AND",
            "LOGI_OR",
            "LOGI_XOR",
            "LOGI_NOT",
            "CMD_END",
            "UNKNOWN_TOKEN",
        };
        Log.verbose(F("%s: %s\n"), PRINT_FUNC, lookup[static_cast<int>(type_)]);
        return lookup[static_cast<int>(type_)];
    }

    /*! 
        @brief  Returns the precedence level of this token.

        @return The precedence level of this token.
                If the token is not an operator, 0 is returned.
    */
    int precedence_level() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        static const int lookup[] = {
            0, // CREATE_NUM
            0, // CREATE_LED
            0, // CREATE_GROUP,
            0, // RUN_GROUP,
            0, // MOVE_BY_FOR,
            0, // MOVE_BY,
            0, // SET_TO_FOR,
            0, // SET_TO,
            0, // NAME,
            0, // NUM_VAL,
            0, // STRING,
            0, // IF,
            0, // ELSE,
            0, // OP_PAREN,
            0, // CL_PAREN,
            0, // COMMA,
            2, // EQUALS,
            2, // L_EQUALS,
            2, // G_EQUALS,
            2, // LESS,
            2, // GREATER,
            3, // MATH_ADD,
            3, // MATH_SUB,
            4, // MATH_MUL,
            4, // MATH_DIV,
            4, // MATH_MOD,
            5, // MATH_POW,
            6, // UNARY_NEG,
            1, // LOGI_AND,
            1, // LOGI_OR,
            1, // LOGI_XOR,
            6, // LOGI_NOT,
            0, // CMD_END,
            0, // UNKNOWN_TOKEN,
        };
        static const int numTypes = sizeof(lookup) / sizeof(lookup[0]);
        Log.verbose(F("%s: %d\n"), PRINT_FUNC, lookup[static_cast<int>(type_)]);
        return lookup[static_cast<int>(type_)];
    }

    /*! 
        @brief  Returns the number of function arguments this token takes.

        @return The number of function arguments for this token.
                If this token is not a function, returns 0.
    */
    int num_function_arguments() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        static const int lookup[] = {
            1, // CREATE_NUM
            2, // CREATE_LED
            0, // CREATE_GROUP,
            1, // RUN_GROUP,
            2, // MOVE_BY_FOR,
            1, // MOVE_BY,
            2, // SET_TO_FOR,
            1, // SET_TO,
            0, // NAME,
            0, // NUM_VAL,
            0, // STRING,
            1, // IF,
            0, // ELSE,
            0, // OP_PAREN,
            0, // CL_PAREN,
            0, // COMMA,
            0, // EQUALS,
            0, // L_EQUALS,
            0, // G_EQUALS,
            0, // LESS,
            0, // GREATER,
            0, // MATH_ADD,
            0, // MATH_SUB,
            0, // MATH_MUL,
            0, // MATH_DIV,
            0, // MATH_MOD,
            0, // MATH_POW,
            0, // UNARY_NEG,
            0, // LOGI_AND,
            0, // LOGI_OR,
            0, // LOGI_XOR,
            0, // LOGI_NOT,
            0, // CMD_END,
            0, // UNKNOWN_TOKEN,
        };
        static const int numTypes = sizeof(lookup) / sizeof(lookup[0]);
        Log.verbose(F("%s: %d\n"), PRINT_FUNC, lookup[static_cast<int>(type_)]);
        return lookup[static_cast<int>(type_)];
    }

    /*!
        @brief  Checks if this token's precedence is greater than another token's precedence.

        @param  other
                The other token to compare against.

        @return True if this token has a higher precedence than the other token, false otherwise.
    */
    bool has_greater_precedence_than(Token const & other) const {
        return precedence_level() > other.precedence_level();
    }

    /*!
        @brief  Checks if this token's precedence equal to another token's precedence.

        @param  other
                The other token to compare against.

        @return True if this token has an equal precedence to the other token, false otherwise.
    */
    bool has_equal_precedence_to(Token const & other) const {
        return precedence_level() == other.precedence_level();
    }

    /*!
        @brief  Checks if this is a CREATE_NUM token.

        @return True if this is a CREATE_NUM token, false otherwise.
    */
    bool is_create_num() const {
        return type_ == TokenType::CREATE_NUM;
    }

    /*!
        @brief  Checks if this is a CREATE_LED token.

        @return True if this is a CREATE_LED token, false otherwise.
    */
    bool is_create_led() const {
        return type_ == TokenType::CREATE_LED;
    }

    /*!
        @brief  Checks if this is a CREATE_GROUP token.

        @return True if this is a CREATE_GROUP token, false otherwise.
    */
    bool is_create_group() const {
        return type_ == TokenType::CREATE_GROUP;
    }

    /*!
        @brief  Checks if this is a RUN_GROUP token.

        @return True if this is a RUN_GROUP token, false otherwise.
    */
    bool is_run_group() const {
        return type_ == TokenType::RUN_GROUP;
    }

    /*!
        @brief  Checks if this is a MOVE_BY_FOR token.

        @return True if this is a MOVE_BY_FOR token, false otherwise.
    */
    bool is_move_by_for() const {
        return type_ == TokenType::MOVE_BY_FOR;
    }

    /*!
        @brief  Checks if this is a MOVE_BY token.

        @return True if this is a MOVE_BY token, false otherwise.
    */
    bool is_move_by() const {
        return type_ == TokenType::MOVE_BY;
    }

    /*!
        @brief  Checks if this is a SET_TO_FOR token.

        @return True if this is a SET_TO_FOR token, false otherwise.
    */
    bool is_set_to_for() const {
        return type_ == TokenType::SET_TO_FOR;
    }

    /*!
        @brief  Checks if this is a SET_TO token.

        @return True if this is a SET_TO token, false otherwise.
    */
    bool is_set_to() const {
        return type_ == TokenType::SET_TO;
    }
    
    /*!
        @brief  Checks if this is a NAME token.

        @return True if this is a NAME token, false otherwise.
    */
    bool is_name() const {
        return type_ == TokenType::NAME;
    }

    /*!
        @brief  Checks if this is a NUM_VAL token.

        @return True if this is a NUM_VAL token, false otherwise.
    */
    bool is_num_val() const {
        return type_ == TokenType::NUM_VAL;
    }
    
    /*!
        @brief  Checks if this is a STRING token.

        @return True if this is a STRING token, false otherwise.
    */
    bool is_string() const {
        return type_ == TokenType::STRING;
    }

    /*!
        @brief  Checks if this is a IF token.

        @return True if this is a IF token, false otherwise.
    */
    bool is_if() const {
        return type_ == TokenType::IF;
    }

    /*!
        @brief  Checks if this is a ELSE token.

        @return True if this is a ELSE token, false otherwise.
    */
    bool is_else() const {
        return type_ == TokenType::ELSE;
    }

    /*!
        @brief  Checks if this is a OP_PAREN token.

        @return True if this is a OP_PAREN token, false otherwise.
    */
    bool is_op_paren() const {
        return type_ == TokenType::OP_PAREN;
    }

    /*!
        @brief  Checks if this is a CL_PAREN token.

        @return True if this is a CL_PAREN token, false otherwise.
    */
    bool is_cl_paren() const {
        return type_ == TokenType::CL_PAREN;
    }

    /*!
        @brief  Checks if this is a COMMA token.

        @return True if this is a COMMA token, false otherwise.
    */
    bool is_comma() const {
        return type_ == TokenType::COMMA;
    }

    /*!
        @brief  Checks if this is a EQUALS token.

        @return True if this is a EQUALS token, false otherwise.
    */
    bool is_equals() const {
        return type_ == TokenType::EQUALS;
    }

    /*!
        @brief  Checks if this is a L_EQUALS token.

        @return True if this is a L_EQUALS token, false otherwise.
    */
    bool is_l_equals() const {
        return type_ == TokenType::L_EQUALS;
    }

    /*!
        @brief  Checks if this is a G_EQUALS token.

        @return True if this is a G_EQUALS token, false otherwise.
    */
    bool is_g_equals() const {
        return type_ == TokenType::G_EQUALS;
    }

    /*!
        @brief  Checks if this is a LESS token.

        @return True if this is a LESS token, false otherwise.
    */
    bool is_less() const {
        return type_ == TokenType::LESS;
    }

    /*!
        @brief  Checks if this is a GREATER token.

        @return True if this is a GREATER token, false otherwise.
    */
    bool is_greater() const {
        return type_ == TokenType::GREATER;
    }

    /*!
        @brief  Checks if this is a MATH_ADD token.

        @return True if this is a MATH_ADD token, false otherwise.
    */
    bool is_math_add() const {
        return type_ == TokenType::MATH_ADD;
    }

    /*!
        @brief  Checks if this is a MATH_SUB token.

        @return True if this is a MATH_SUB token, false otherwise.
    */
    bool is_math_sub() const {
        return type_ == TokenType::MATH_SUB;
    }

    /*!
        @brief  Checks if this is a MATH_MUL token.

        @return True if this is a MATH_MUL token, false otherwise.
    */
    bool is_math_mul() const {
        return type_ == TokenType::MATH_MUL;
    }

    /*!
        @brief  Checks if this is a MATH_DIV token.

        @return True if this is a MATH_DIV token, false otherwise.
    */
    bool is_math_div() const {
        return type_ == TokenType::MATH_DIV;
    }

    /*!
        @brief  Checks if this is a MATH_MOD token.

        @return True if this is a MATH_MOD token, false otherwise.
    */
    bool is_math_mod() const {
        return type_ == TokenType::MATH_MOD;
    }

    /*!
        @brief  Checks if this is a MATH_POW token.

        @return True if this is a MATH_POW token, false otherwise.
    */
    bool is_math_pow() const {
        return type_ == TokenType::MATH_POW;
    }

    /*!
        @brief  Checks if this is a UNARY_NEG token.

        @return True if this is a UNARY_NEG token, false otherwise.
    */
    bool is_unary_neg() const {
        return type_ == TokenType::UNARY_NEG;
    }

    /*!
        @brief  Checks if this is a LOGI_AND token.

        @return True if this is a LOGI_AND token, false otherwise.
    */
    bool is_logi_and() const {
        return type_ == TokenType::LOGI_AND;
    }

    /*!
        @brief  Checks if this is a LOGI_OR token.

        @return True if this is a LOGI_OR token, false otherwise.
    */
    bool is_logi_or() const {
        return type_ == TokenType::LOGI_OR;
    }

    /*!
        @brief  Checks if this is a LOGI_XOR token.

        @return True if this is a LOGI_XOR token, false otherwise.
    */
    bool is_logi_xor() const {
        return type_ == TokenType::LOGI_XOR;
    }

    /*!
        @brief  Checks if this is a LOGI_NOT token.

        @return True if this is a LOGI_NOT token, false otherwise.
    */
    bool is_logi_not() const {
        return type_ == TokenType::LOGI_NOT;
    }

    /*!
        @brief  Checks if this is a CMD_END token.

        @return True if this is a CMD_END token, false otherwise.
    */
    bool is_cmd_end() const {
        return type_ == TokenType::CMD_END;
    }

    /*!
        @brief  Checks if this is a UNKNOWN_TOKEN token.

        @return True if this is a UNKNOWN_TOKEN token, false otherwise.
    */
    bool is_unknown_token() const {
        return type_ == TokenType::UNKNOWN_TOKEN;
    }

    /*!
        @brief  Checks if this token is a create command.

        @return True if this token is a create command, false otherwise.
    */
    bool is_create_command() const {
        return is_create_num() || is_create_led() || is_create_group();
    }

    /*!
        @brief  Checks if this token is a move by command.

        @return True if this token is a move by command, false otherwise.
    */
    bool is_move_by_command() const {
        return is_move_by_for() || is_move_by();
    }

    /*!
        @brief  Checks if this token is a set to command.

        @return True if this token is a set to command, false otherwise.
    */
    bool is_set_to_command() const {
        return is_set_to_for() || is_set_to();
    }

    /*!
        @brief  Checks if this token is an operand.

        @return True if this token is an operand, false otherwise.
    */
    bool is_operand() const {
        return is_name() || is_num_val();
    }

    /*!
        @brief Checks if this token is a conditional command.

        @return True if this token is a conditional command, false otherwise.
    */
    bool is_conditional_command() const {
        return is_if() || is_else();
    }

    /*!
        @brief  Checks if this token is a unary operator.

        @return True if this token is a unary operator, false otherwise.
    */
    bool is_unary_operator() const {
        return is_unary_neg() || is_logi_not();
    }

    /*!
        @brief  Checks if this token is a binary operator.

        @return True if this token is a binary operator, false otherwise.
    */
    bool is_binary_operator() const {
        return is_equals() || is_l_equals() || is_g_equals() || is_less() || is_greater() ||
               is_math_add() || is_math_sub() || is_math_mul() || is_math_div() || is_math_mod() || is_math_pow() ||
               is_logi_and() || is_logi_or() || is_logi_xor();
    }

    /*!
        @brief  Checks if this token is an operator.

        @return True if this token is an operator, false otherwise.
    */
    bool is_operator() const {
        return is_unary_operator() || is_binary_operator();
    }

    /*!
        @brief  Checks if this token is a left associative operator.

        @return True if this token is a left associative operator, false otherwise.
    */
    bool is_left_associative() const {
        return is_operator() && !is_math_pow();
    }

    /*!
        @brief  Checks if this token is a function.

        @return True if this token is a function, false otherwise.
    */
    bool is_function() const {
        return is_create_command() || is_run_group() || 
               is_move_by_command() || is_set_to_command() ||
               is_conditional_command();
    }

private:
    TokenType type_;
    PoolString<> value_;

};

/*!
    @brief  Converts a command word string to its corresponding token type.

    @param  str
            The command word.

    @return The corresponding token type for the command word.
            If the parameter is not a valid command word,
            the unknown token type is returned.
*/
TokenType command_str_to_token_type(char const * str) {
    Log.verbose(F("%s\n"), PRINT_FUNC);
    TokenType tokenType = TokenType::UNKNOWN_TOKEN;
    if (::strlen(str) == 0) {
        Log.warning(F("%s: empty string\n"), PRINT_FUNC);
        return TokenType::UNKNOWN_TOKEN;
    }
    static const char lookup[][10] = {
        "IsNumber",
        "IsLED",
        "IsGroup",
        "RunGroup",
        "MoveByFor",
        "MoveBy",
        "SetToFor",
        "SetTo",
        "",
        "",
        "",
        "If",
        "Else",
    };
    static const int numTypes = sizeof(lookup) / sizeof(lookup[0]);
    for (int i = 0; i < numTypes; ++i) {
        if (std::strcmp(str, lookup[i]) == 0) {
            Log.verbose(F("%s: %d\n"), PRINT_FUNC, static_cast<TokenType>(i));
            tokenType = static_cast<TokenType>(i);
        }
    }
    if (tokenType == TokenType::UNKNOWN_TOKEN) {
        Log.warning(F("%s: unknown command string %s\n"), PRINT_FUNC, str);
    }
    return tokenType;
}

/*!
    @brief  Converts a command word string to its corresponding token type.

    @param  str
            The command word.

    @return The corresponding token type for the command word.
            If the parameter is not a valid command word,
            the unknown token type is returned.
*/
TokenType command_str_to_token_type(PoolString<> const & str) {
    Log.verbose(F("%s\n"), PRINT_FUNC);
    return command_str_to_token_type(str.c_str());

    TokenType tokenType = TokenType::UNKNOWN_TOKEN;
    if (str.strlen() == 0) {
        Log.warning(F("%s: empty string\n"), PRINT_FUNC);
        return TokenType::UNKNOWN_TOKEN;
    }
    static const char lookup[][10] = {
        "IsNumber",
        "IsLED",
        "IsGroup",
        "RunGroup",
        "MoveByFor",
        "MoveBy",
        "SetToFor",
        "SetTo",
        "",
        "",
        "",
        "If",
        "Else",
    };
    static const int numTypes = sizeof(lookup) / sizeof(lookup[0]);
    for (int i = 0; i < numTypes; ++i) {
        if (str == lookup[i]) {
            Log.verbose(F("%s: %d\n"), PRINT_FUNC, static_cast<TokenType>(i));
            tokenType = static_cast<TokenType>(i);
        }
    }
    if (tokenType == TokenType::UNKNOWN_TOKEN) {
        Log.warning(F("%s: unknown command string %s\n"), PRINT_FUNC, str.c_str());
    }
    return tokenType;
}

/*!
    @brief  Converts a punctuation character to its corresponding token type.

    @param  ch
            The punctuation.

    @return The corresponding token type for the punctuation.
            If the parameter is not valid punctuation,
            the unknown token type is returned.
*/
TokenType punctuation_char_to_token_type(char const & ch) {
    Log.verbose(F("%s\n"), PRINT_FUNC);
    switch (ch) {
    case '(':
        return TokenType::OP_PAREN;
    case ')':
        return TokenType::CL_PAREN;
    case ',':
        return TokenType::COMMA;
    case '=':
        return TokenType::EQUALS;
    case '<':
        return TokenType::LESS;
    case '>':
        return TokenType::GREATER;
    case '+':
        return TokenType::MATH_ADD;
    case '-':
        return TokenType::MATH_SUB;
    case '*':
        return TokenType::MATH_MUL;
    case '/':
        return TokenType::MATH_DIV;
    case '%':
        return TokenType::MATH_MOD;
    case '^':
        return TokenType::MATH_POW;
    case '&':
        return TokenType::LOGI_AND;
    case '|':
        return TokenType::LOGI_OR;
    case '!':
        return TokenType::LOGI_XOR;
    case '~':
        return TokenType::LOGI_NOT;
    };
    Log.warning(F("%s: unknown token %c\n"), PRINT_FUNC, ch);
    return TokenType::UNKNOWN_TOKEN;
}

} // namespace kty
