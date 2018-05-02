#pragma once

#include <kty/stl_impl.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <map>
#include <vector>

#include <kty/string_utils.hpp>

namespace kty {

// Not using enum class due to int conversion requirement for ArduinoUnit
/** The various types of tokens possible */
enum TokenType {
    CREATE_NUM, CREATE_LED, CREATE_SERVO, CREATE_GROUP, RUN_GROUP,
    MOVE_BY_FOR, MOVE_BY, SET_TO_FOR, SET_TO,
    NAME, NUM_VAL, STRING,
    IF, ELSE_IF, ELSE, WHILE, 
    OP_PAREN, CL_PAREN, COMMA,
    EQUALS, L_EQUALS, G_EQUALS, LESS, GREATER,
    MATH_ADD, MATH_SUB, MATH_MUL, MATH_DIV, MATH_MOD, MATH_POW,
    UNARY_NEG,
    LOGI_AND, LOGI_OR, LOGI_XOR, LOGI_NOT,
    UNKNOWN_TYPE,
    CMD_END,
};

/*!
    @brief  Converts a token type to its corresponding string representation.

    @param  tokenType
            The token type to be converted.

    @return The string representation of the token type.
            If the token type is invalid, "UNKNOWN" is returned.
*/
std::string token_type_to_str(TokenType tokenType) {
    static const std::map<TokenType, std::string> lookupMap = {
        {TokenType::CREATE_NUM, "CREATE_NUM"},
        {TokenType::CREATE_LED, "CREATE_LED"},
        {TokenType::CREATE_SERVO, "CREATE_SERVO"},
        {TokenType::CREATE_GROUP, "CREATE_GROUP"},
        {TokenType::RUN_GROUP, "RUN_GROUP"},
        {TokenType::MOVE_BY_FOR, "MOVE_BY_FOR"},
        {TokenType::MOVE_BY, "MOVE_BY"},
        {TokenType::SET_TO_FOR, "SET_TO_FOR"},
        {TokenType::SET_TO, "SET_TO"},
        {TokenType::NAME, "NAME"},
        {TokenType::NUM_VAL, "NUM_VAL"},
        {TokenType::STRING, "STRING"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE_IF, "ELSE_IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::OP_PAREN, "OP_PAREN"},
        {TokenType::CL_PAREN, "CL_PAREN"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::EQUALS, "EQUALS"},
        {TokenType::L_EQUALS, "L_EQUALS"},
        {TokenType::G_EQUALS, "G_EQUALS"},
        {TokenType::LESS, "LESS"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::MATH_ADD, "MATH_ADD"},
        {TokenType::MATH_SUB, "MATH_SUB"},
        {TokenType::MATH_MUL, "MATH_MUL"},
        {TokenType::MATH_DIV, "MATH_DIV"},
        {TokenType::MATH_MOD, "MATH_MOD"},
        {TokenType::MATH_POW, "MATH_POW"},
        {TokenType::UNARY_NEG, "UNARY_NEG"},
        {TokenType::LOGI_AND, "LOGI_AND"},
        {TokenType::LOGI_OR, "LOGI_OR"},
        {TokenType::LOGI_XOR, "LOGI_XOR"},
        {TokenType::LOGI_NOT, "LOGI_NOT"},
        {TokenType::UNKNOWN_TYPE, "UNKNOWN"},
        {TokenType::CMD_END, "CMD_END"},
    };
    if (lookupMap.find(tokenType) == lookupMap.end()) {
        return "UNKNOWN";
    }
    return lookupMap.find(tokenType)->second;
}

/*!
    @brief  Converts a command word string to its corresponding token type.

    @param  str
            The command word.

    @return The corresponding token type for the command word.
            If the parameter is not a valid command word,
            the unknown token type is returned.
*/
TokenType command_str_to_token_type(std::string const & str) {
    static const std::map<std::string, TokenType> lookupMap = {
        {"IsNumber", TokenType::CREATE_NUM},
        {"IsLED", TokenType::CREATE_LED},
        {"IsServo", TokenType::CREATE_SERVO},
        {"IsGroup", TokenType::CREATE_GROUP},
        {"RunGroup", TokenType::RUN_GROUP},
        {"MoveByFor", TokenType::MOVE_BY_FOR},
        {"MoveBy", TokenType::MOVE_BY},
        {"SetToFor", TokenType::SET_TO_FOR},
        {"SetTo", TokenType::SET_TO},
        {"If", TokenType::IF},
        {"ElseIf", TokenType::ELSE_IF},
        {"Else", TokenType::ELSE},
        {"While", TokenType::WHILE},
    };
    if (lookupMap.find(str) == lookupMap.end()) {
        return TokenType::UNKNOWN_TYPE;
    }
    return lookupMap.find(str)->second;
}

/*!
    @brief  Converts a punctuation string to its corresponding token type.

    @param  str
            The punctuation.

    @return The corresponding token type for the punctuation.
            If the parameter is not valid punctuation,
            the unknown token type is returned.
*/
TokenType punctuation_str_to_token_type(std::string const & str) {
    static const std::string validPunctuation_ = "(),=<>+-*/%^&|!~";
    static const TokenType lookup[] = {
        TokenType::OP_PAREN,
        TokenType::CL_PAREN,
        TokenType::COMMA,
        TokenType::EQUALS,
        TokenType::LESS,
        TokenType::GREATER,
        TokenType::MATH_ADD,
        TokenType::MATH_SUB,
        TokenType::MATH_MUL,
        TokenType::MATH_DIV,
        TokenType::MATH_MOD,
        TokenType::MATH_POW,
        TokenType::LOGI_AND,
        TokenType::LOGI_OR,
        TokenType::LOGI_XOR,
        TokenType::LOGI_NOT,
    };
    int idx = validPunctuation_.find(str);
    if (idx == -1) {
        return TokenType::UNKNOWN_TYPE;
    }
    return lookup[idx];
}

struct Token;

int get_token_precedence_level(Token const & token);

/*!
    @brief  Class that holds information about a token
*/
struct Token {
    /** The type of the token */
    TokenType type;
    /** The value of the token */
    std::string value;

    /*!
        @brief  The constructor for a token.

        @param  _type
                The type of the token

        @param  _value
                The value of the token. Optional parameter.
    */
    Token(TokenType _type, std::string const & _value = "") : type(_type), value(_value) {
    }

    /*!
        @brief  The constructor for a token.

        @param  _type
                The type of the token

        @param  _value
                The value of the token.
    */
    Token(TokenType _type, char const * _value) : type(_type), value(_value) {
    }

    /*!
        @brief  Gets the string representation of the token for debugging

        @return The debugging string representation of the token.
    */
    std::string str() const {
        return "Token(" + token_type_to_str(type) + ", " + value + ")";
    }

    /*!
        @brief  Checks if this token is a number.

        @return True if this token is an number, false otherwise.
    */
    bool is_number() const {
        return type == TokenType::NUM_VAL;
    }

    /*!
        @brief  Checks if this token is a name.

        @return True if this token is a name, false otherwise.
    */
    bool is_name() const {
        return type == TokenType::NAME;
    }

    /*!
        @brief  Checks if this token is an operand.

        @return True if this token is an operand, false otherwise.
    */
    bool is_operand() const {
        return type == TokenType::NAME || 
               type == TokenType::NUM_VAL;
    }

    /*!
        @brief  Checks if this token is a string.

        @return True if this token is a string, false otherwise.
    */
    bool is_string() const {
        return type == TokenType::STRING;
    }

    /*!
        @brief  Checks if this token is the create number command.

        @return True if this token is the create number command, false otherwise.
    */
    bool is_create_num() const {
        return type == TokenType::CREATE_NUM;
    }

    /*!
        @brief  Checks if this token is the create led command.

        @return True if this token is the create led command, false otherwise.
    */
    bool is_create_led() const {
        return type == TokenType::CREATE_LED;
    }

    /*!
        @brief  Checks if this token is the create servo command.

        @return True if this token is the create servo command, false otherwise.
    */
    bool is_create_servo() const {
        return type == TokenType::CREATE_SERVO;
    }

    /*!
        @brief  Checks if this token is the create group command.

        @return True if this token is the create group command, false otherwise.
    */
    bool is_create_group() const {
        return type == TokenType::CREATE_GROUP;
    }

    /*!
        @brief  Checks if this token is the if command.

        @return True if this token is the if command, false otherwise.
    */
    bool is_if() const {
        return type == TokenType::IF;
    }

    /*!
        @brief  Checks if this token is the else connand.

        @return True if this token is the create else command, false otherwise.
    */
    bool is_else() const {
        return type == TokenType::ELSE;
    }

    /*!
        @brief  Checks if this token is a function.

        @return True if this token is a function, false otherwise.
    */
    bool is_function() const {
        return type == TokenType::CREATE_NUM ||
               type == TokenType::CREATE_LED ||
               type == TokenType::CREATE_SERVO ||
               type == TokenType::CREATE_GROUP ||
               type == TokenType::RUN_GROUP ||
               type == TokenType::MOVE_BY_FOR ||
               type == TokenType::MOVE_BY ||
               type == TokenType::SET_TO_FOR ||
               type == TokenType::SET_TO ||
               type == TokenType::IF ||
               type == TokenType::ELSE_IF ||
               type == TokenType::ELSE ||
               type == TokenType::WHILE;
    }

    /*!
        @brief  Checks if this token is a unary operator.

        @return True if this token is a unary operator, false otherwise.
    */
    bool is_unary_operator() const {
        return type == TokenType::UNARY_NEG || 
               type == TokenType::LOGI_NOT;
    }

    /*!
        @brief  Checks if this token is an operator.

        @return True if this token is an operator, false otherwise.
    */
    bool is_operator() const {
        return type == TokenType::EQUALS || 
               type == TokenType::L_EQUALS || 
               type == TokenType::G_EQUALS || 
               type == TokenType::LESS || 
               type == TokenType::GREATER || 
               type == TokenType::MATH_ADD || 
               type == TokenType::MATH_SUB || 
               type == TokenType::MATH_MUL || 
               type == TokenType::MATH_DIV || 
               type == TokenType::MATH_MOD || 
               type == TokenType::MATH_POW ||
               type == TokenType::UNARY_NEG ||
               type == TokenType::LOGI_AND ||
               type == TokenType::LOGI_OR ||
               type == TokenType::LOGI_XOR ||
               type == TokenType::LOGI_NOT;
    }
    
    /*!
        @brief  Checks if this token is a left bracket.

        @return True if this token is a left bracket, false otherwise.
    */
    bool is_left_bracket() const {
        return type == TokenType::OP_PAREN;
    }

    /*!
        @brief  Checks if this token is a right bracket.

        @return True if this token is a right bracket, false otherwise.
    */
    bool is_right_bracket() const {
        return type == TokenType::CL_PAREN;
    }

    /*!
        @brief  Checks if this token is a comma.

        @return True if this token is a comma, false otherwise.
    */
    bool is_comma() const {
        return type == TokenType::COMMA;
    }

    /*!
        @brief  Checks if this token is a creation command type.

        @return True if this token is a creation command type, false otherwise.
    */
    bool is_creation_command() const {
        return type == TokenType::CREATE_NUM || 
               type == TokenType::CREATE_LED || 
               type == TokenType::CREATE_SERVO || 
               type == TokenType::CREATE_GROUP;
    }

    /*!
        @brief  Checks if this token is a left associative operator.

        @return True if this token is a left associative operator, false otherwise.
    */
    bool is_left_associative() const {
        return is_operator() && type != TokenType::MATH_POW;
    }

    /*!
        @brief  Checks if this token's precedence is greater than another token's precedence.

        @param  other
                The other token to compare against.

        @return True if this token has a higher precedence than the other token, false otherwise.
    */
    bool has_greater_precedence_than(Token const & other) const {
        return get_token_precedence_level(*this) > get_token_precedence_level(other);
    }

    /*!
        @brief  Checks if this token's precedence equal to another token's precedence.

        @param  other
                The other token to compare against.

        @return True if this token has an equal precedence to the other token, false otherwise.
    */
    bool has_equal_precedence_to(Token const & other) const {
        return get_token_precedence_level(*this) == get_token_precedence_level(other);
    }

    /*! 
        @brief  Returns the number of function arguments this token takes.

        @return The number of function arguments for this token.
                If this token is not a function, returns 0.
    */
    int num_function_arguments() const {
        // TODO: convert to static const map
        switch (type) {
        case TokenType::CREATE_NUM:
            return 1;
        case TokenType::CREATE_LED:
            return 2;
        case TokenType::CREATE_SERVO:
            return 2;
        case TokenType::CREATE_GROUP:
            return 0;
        case TokenType::RUN_GROUP:
            return 1;
        case TokenType::MOVE_BY_FOR:
            return 2;
        case TokenType::MOVE_BY:
            return 1;
        case TokenType::SET_TO_FOR:
            return 2;
        case TokenType::SET_TO:
            return 1;
        case TokenType::IF:
            return 1;
        case TokenType::ELSE_IF:
            return 1;
        case TokenType::ELSE:
            return 0;
        case TokenType::WHILE:
            return 1;
        default:
            return 0;
        }
    }
};

/*! 
    @brief  Returns the precedence level of a token.

    @param  token
            The token to check the precedence level of.

    @return The precedence level of the token.
            If the token is not an operator, 0 is returned.
*/
int get_token_precedence_level(Token const & token) {
    static const std::map<TokenType, int> precedenceLevel = {
        {TokenType::UNARY_NEG, 6},
        {TokenType::LOGI_NOT, 6},
        {TokenType::MATH_POW, 5},
        {TokenType::MATH_MUL, 4},
        {TokenType::MATH_DIV, 4},
        {TokenType::MATH_MOD, 4},
        {TokenType::MATH_ADD, 3},
        {TokenType::MATH_SUB, 3},
        {TokenType::EQUALS, 2},
        {TokenType::L_EQUALS, 2},
        {TokenType::G_EQUALS, 2},
        {TokenType::LESS, 2},
        {TokenType::GREATER, 2},
        {TokenType::LOGI_AND, 1},
        {TokenType::LOGI_OR, 1},
        {TokenType::LOGI_XOR, 1},
    };
    if (precedenceLevel.find(token.type) == precedenceLevel.end()) {
        return 0;
    }
    return precedenceLevel.find(token.type)->second;
}

/*!
    @brief  Class that tokenizes commands.
*/
class Tokenizer {

public:

    /*!
        @brief  Default constructor for tokenizer.
    */
    Tokenizer() {
    }

    /*!
        @brief  Constructor for tokenizer that takes in command to tokenize.

        @param  command
                The command to tokenize.
    */
    explicit Tokenizer(std::string const & command) {
        set_command(command);
    }

    /*!
        @brief  Sets the command to be tokenized.

        @param  command
                The command to tokenize.
    */
    void set_command(std::string const & command) {
        command_ = remove_str_whitespace(command);
        add_missing_optional_arguments();
        tokenStartIdx_ = 0;
    }

    /*!
        @brief  Tokenizes the command stored in the tokenizer.

        @return The tokenized command.
    */
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        Token token(TokenType::UNKNOWN_TYPE);
        do {
            token = get_next_token();
            tokens.push_back(token);
        } while (token.type != TokenType::CMD_END && token.type != TokenType::UNKNOWN_TYPE);
        process_math_tokens(tokens);
        return tokens;
    }

    /*!
        @brief  Tokenizes the given command.

        @param  command
                The command to tokenize.

        @return The tokenized command.
    */
    std::vector<Token> tokenize(std::string const & command) {
        set_command(command);
        return tokenize();
    }

    /*!
        @brief  Performs processing on the math tokens within the command.
                Comparison tokens are compressed, and unary '-' tokens are detected.

        @param  tokens
                The tokenized command.
    */
    void process_math_tokens(std::vector<Token> & tokens) {
        // Compresses '<' + '=' into '<=' and '>' + '=' into '>='
        std::vector<Token>::iterator iter = tokens.begin();
        while (iter != tokens.end()) {
            std::vector<Token>::iterator next = iter;
            ++next;
            if (next != tokens.end()) {
                if (iter->type == TokenType::LESS) {
                    if (next->type == TokenType::EQUALS) {
                        iter->type = TokenType::L_EQUALS;
                        next = tokens.erase(next);
                    }
                }
                else if (iter->type == TokenType::GREATER) {
                    if (next->type == TokenType::EQUALS) {
                        iter->type = TokenType::G_EQUALS;
                        next = tokens.erase(next);
                    }
                }
            }
            iter = next;
        }

        // Detects unary '-' tokens
        for (std::vector<Token>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
            if (iter->type == TokenType::MATH_SUB) {
                std::vector<Token>::iterator prev = iter;
                --prev;
                if (iter == tokens.begin() ||
                    prev->is_operator() ||
                    prev->is_left_bracket()) {
                    iter->type = TokenType::UNARY_NEG;
                }
            }
        }
    }

    /*!
        @brief  Gets the next token from command stored in the tokenizer.

        @return The next token from the stored command.
    */
    Token get_next_token() {
        // No more tokens
        if (tokenStartIdx_ == command_.size()) {
            return Token(TokenType::CMD_END);
        }
        // Next token is command word
        if (isupper(command_[tokenStartIdx_])) {
            return get_next_command_token();
        }
        // Next token is name
        else if (islower(command_[tokenStartIdx_]) || command_[tokenStartIdx_] == '_') {
            return get_next_name_token();
        }
        // Next token is number
        else if (isdigit(command_[tokenStartIdx_])) {
            return get_next_number_token();
        }
        // Next token is string
        else if (command_[tokenStartIdx_] == '"') {
            return get_next_string_token('"');
        }
        else if (command_[tokenStartIdx_] == '\'') {
            return get_next_string_token('\'');
        }
        // Next token is punctuation
        else if (validPunctuation_.find(command_[tokenStartIdx_]) != std::string::npos) {
            return get_next_punctuation_token();
        }
        return Token(TokenType::UNKNOWN_TYPE);
    }

    /*!
        @brief  Gets the next command token from the stored command.

        @return The next command token.
                If the next token is not a command, an unknown token is returned.
    */
    Token get_next_command_token() {
        for (int i = 0; i < commandLookup_.size(); ++i) {
            if (command_.find(commandLookup_[i], tokenStartIdx_) == tokenStartIdx_) {
                tokenStartIdx_ += commandLookup_[i].size();
                return Token(command_str_to_token_type(commandLookup_[i]));
            }
        }
        return Token(TokenType::UNKNOWN_TYPE);
    }

    /*!
        @brief  Gets the next name token from the stored command.

        @return The next name token.
                If the next token is not a name, an unknown token is returned.
    */
    Token get_next_name_token() {
        int currIdx = tokenStartIdx_;
        while (currIdx < command_.size() && (islower(command_[currIdx]) || command_[currIdx] == '_')) {
            ++currIdx;
        }
        std::string value = command_.substr(tokenStartIdx_, currIdx - tokenStartIdx_);
        Token result(TokenType::NAME, value);
        tokenStartIdx_ = currIdx;
        return result;
    }

    /*!
        @brief  Gets the next number token from the stored command.

        @return The next number token.
                If the next token is not a number, an unknown token is returned.
    */
    Token get_next_number_token() {
        int currIdx = tokenStartIdx_;
        while (currIdx < command_.size() && isdigit(command_[currIdx])) {
            ++currIdx;
        }
        std::string value = command_.substr(tokenStartIdx_, currIdx - tokenStartIdx_);
        Token result(TokenType::NUM_VAL, value);
        tokenStartIdx_ = currIdx;
        return result;
    }

    /*!
        @brief  Gets the next string token from the stored command.

        @param  open
                The opening character to the string.
                The closing character must match the opening character.

        @return The next string token.
                If the next token is not a string, an unknown token is returned.
    */
    Token get_next_string_token(char const & open) {
        int endIdx = command_.find(open, tokenStartIdx_ + 1);
        // Only take substr of the string, without quotes
        Token result(TokenType::STRING, command_.substr(tokenStartIdx_ + 1, endIdx - tokenStartIdx_ - 1));
        tokenStartIdx_ = endIdx + 1;
        return result;
    }

    /*!
        @brief  Gets the next punctuation token from the stored command.

        @return The next punctuation token.
                If the next token is not punctuation, an unknown token is returned.
    */
    Token get_next_punctuation_token() {
        Token result(punctuation_str_to_token_type(command_.substr(tokenStartIdx_, 1)));
        ++tokenStartIdx_;
        return result;    
    }

    /*!
        @brief  Gets the missing arguments, if any, for a function.

        @param  tokenType
                The type of the function

        @param  numArguments
                The number of arguments to the function supplied by the user.

        @return The remaining missing arguments, if any.
    */
    std::string get_additional_arguments(TokenType tokenType, int const & numArguments) {
        switch (tokenType) {
        case TokenType::CREATE_NUM:
            // Only can be missing 1 argument
            return "0";
        case TokenType::CREATE_LED:
            // Only can be missing 1 argument
            return ",50";
        case TokenType::CREATE_SERVO:
            // Only can be missing 1 argument
            return ",90";
        case TokenType::CREATE_GROUP:
            // No arguments
            return "";
        case TokenType::RUN_GROUP:
            // Only can be missing 1 argument
            return "1";
        case TokenType::MOVE_BY_FOR:
            // Cannot be missing arguments
            return "";
        case TokenType::MOVE_BY:
            // Cannot be missing arguments
            return "";
        case TokenType::SET_TO_FOR:
            // Cannot be missing arguments
            return "";
        case TokenType::SET_TO:
            // Cannot be missing arguments
            return "";
        case TokenType::IF:
            // Cannot be missing arguments
            return "";
        case TokenType::ELSE_IF:
            // Cannot be missing arguments
            return "";
        case TokenType::ELSE:
            // No arguments
            return "";
        case TokenType::WHILE:
            // Cannot be missing arguments
            return "";
        default:
            return "";
        }
    }

    /*!
        @brief  Adds the missing arguments, if any, for the functions present in the command.
    */
    void add_missing_optional_arguments() {
        for (int i = 0; i < commandLookup_.size(); ++i) {
            int idx = command_.find(commandLookup_[i]);
            // This command word isn't in the command
            if (idx == std::string::npos) {
                continue;
            }
            int opParenIdx = command_.find('(', idx + commandLookup_[i].size());
            int clParenIdx = command_.find(')', opParenIdx);
            int numArguments = 0;
            // At least one argument between them
            if (clParenIdx > opParenIdx + 1) {
                numArguments = std::count(command_.begin() + opParenIdx, command_.begin() + clParenIdx, ',') + 1;
            }
            // Need to fill up arguments
            if (numArguments < Token(command_str_to_token_type(commandLookup_[i])).num_function_arguments()) {
                std::string aditionalArguments = get_additional_arguments(command_str_to_token_type(commandLookup_[i]), numArguments);
                command_.insert(clParenIdx, aditionalArguments);
            }
        }
    }

private:
    std::string command_;
    int tokenStartIdx_ = 0;
    std::vector<std::string> commandLookup_ = {
        "IsNumber",
        "IsLED",
        "IsServo",
        "IsGroup",
        "RunGroup",
        "MoveByFor",
        "MoveBy",
        "SetToFor",
        "SetTo",
        "If",
        "ElseIf",
        "Else",
        "While"
    };
    const std::string validPunctuation_ = "(),=<>+-*/%^&|!~";
};

} // namespace kty
