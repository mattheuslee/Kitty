#pragma once

#include <kitty/stl_impl.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <map>
#include <vector>

#include <kitty/utils/string_utils.hpp>

namespace kitty {

namespace tokenizer {

// Not using enum class due to int conversion requirement for ArduinoUnit
enum TokenType {
    CREATE_NUM, CREATE_LED, CREATE_SERVO, CREATE_GROUP, RUN_GROUP,
    MOVE_BY_FOR, MOVE_BY, SET_TO_FOR, SET_TO,
    NAME, NUM_VAL, 
    IF, ELSE_IF, ELSE, WHILE, 
    OP_PAREN, CL_PAREN, COMMA,
    EQUALS, L_EQUALS, G_EQUALS, LESS, GREATER,
    MATH_ADD, MATH_SUB, MATH_MUL, MATH_DIV, MATH_MOD, MATH_POW,
    UNARY_NEG,
    LOGI_AND, LOGI_OR, LOGI_XOR,
    UNKNOWN,
    CMD_END,
};

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
        {TokenType::UNKNOWN, "UNKNOWN"},
        {TokenType::CMD_END, "CMD_END"},
    };
    if (lookupMap.find(tokenType) == lookupMap.end()) {
        return "UNKNOWN";
    }
    return lookupMap[tokenType];
}

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
        return TokenType::UNKNOWN;
    }
    return lookupMap[str];
}

TokenType punctuation_str_to_token_type(std::string const & str) {
    static const std::string validPunctuation_ = "(),=<>+-*/%^&|!";
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
    };
    int idx = validPunctuation_.find(str);
    if (idx == -1) {
        return TokenType::UNKNOWN;
    }
    return lookup[idx];
}

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType _type, std::string const & _value = "") : type(_type), value(_value) {
    }
    Token(TokenType _type, char const * _value) : type(_type), value(_value) {
    }
    std::string str() const {
        return "Token(" + token_type_to_str(type) + ", " + value + ")";
    }
};

bool is_operand(Token const & token) {
    return token.type == TokenType::NAME || 
           token.type == TokenType::NUM_VAL;
}

bool is_function(Token const & token) {
    return token.type == TokenType::CREATE_NUM ||
           token.type == TokenType::CREATE_LED ||
           token.type == TokenType::CREATE_SERVO ||
           token.type == TokenType::CREATE_GROUP ||
           token.type == TokenType::RUN_GROUP ||
           token.type == TokenType::MOVE_BY_FOR ||
           token.type == TokenType::MOVE_BY ||
           token.type == TokenType::SET_TO_FOR ||
           token.type == TokenType::SET_TO ||
           token.type == TokenType::IF ||
           token.type == TokenType::ELSE_IF ||
           token.type == TokenType::ELSE ||
           token.type == TokenType::WHILE;
}

bool is_operator(Token const & token) {
    return token.type == TokenType::EQUALS || 
           token.type == TokenType::L_EQUALS || 
           token.type == TokenType::G_EQUALS || 
           token.type == TokenType::LESS || 
           token.type == TokenType::GREATER || 
           token.type == TokenType::MATH_ADD || 
           token.type == TokenType::MATH_SUB || 
           token.type == TokenType::MATH_MUL || 
           token.type == TokenType::MATH_DIV || 
           token.type == TokenType::MATH_MOD || 
           token.type == TokenType::MATH_POW ||
           token.type == TokenType::UNARY_NEG ||
           token.type == TokenType::LOGI_AND ||
           token.type == TokenType::LOGI_OR ||
           token.type == TokenType::LOGI_XOR;
}

bool is_left_bracket(Token const & token) {
    return token.type == TokenType::OP_PAREN;
}

bool is_right_bracket(Token const & token) {
    return token.type == TokenType::CL_PAREN;
}

bool is_comma(Token const & token) {
    return token.type == TokenType::COMMA;
}

bool is_creation(Token const & token) {
    return token.type == TokenType::CREATE_NUM || 
           token.type == TokenType::CREATE_LED || 
           token.type == TokenType::CREATE_SERVO || 
           token.type == TokenType::CREATE_GROUP;
}

int get_precedence_level(Token const & token) {
    static const std::map<TokenType, int> precedenceLevel = {
        {TokenType::UNARY_NEG, 6},
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
    return precedenceLevel[token.type];
}

// Returns whether left >(precedence) right is true
bool has_greater_precedence(Token const & left, Token const & right) {
    return get_precedence_level(left) > get_precedence_level(right);
}

// Returns whether left ==(precedence) right is true
bool has_equal_precedence(Token const & left, Token const & right) {
    return get_precedence_level(left) == get_precedence_level(right);
}

bool is_left_associative(Token const & token) {
    return token.type != TokenType::MATH_POW;
}

int num_function_arguments(Token const & token) {
    // Number of function arguments includes optional arguments
    // Assume optional arguments are filled in before passing through interpreter
    switch (token.type) {
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

int num_function_arguments(TokenType tokenType) {
    return num_function_arguments(Token(tokenType));
}

class Tokenizer {

public:
    Tokenizer() {
    }

    explicit Tokenizer(std::string & command) {
        set_command(command);
    }

    void set_command(std::string & command) {
        command_ = utils::remove_str_whitespace(command);
        add_missing_optional_arguments();
        tokenStartIdx_ = 0;
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        Token token(TokenType::UNKNOWN);
        do {
            token = get_next_token();
            tokens.push_back(token);
        } while (token.type != TokenType::CMD_END && token.type != TokenType::UNKNOWN);
        cleanup_math_tokens(tokens);
        return tokens;
    }

    std::vector<Token> tokenize(std::string & command) {
        set_command(command);
        return tokenize();
    }

    void cleanup_math_tokens(std::vector<Token> & tokens) {
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
                    is_operator(*prev) ||
                    is_left_bracket(*prev)) {
                    iter->type = TokenType::UNARY_NEG;
                }
            }
        }
    }

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
        // Next token is punctuation
        else if (validPunctuation_.find(command_[tokenStartIdx_]) != std::string::npos) {
            return get_next_punctuation_token();
        }
        return Token(TokenType::UNKNOWN);
    }

    Token get_next_command_token() {
        for (int i = 0; i < commandLookup_.size(); ++i) {
            if (command_.find(commandLookup_[i], tokenStartIdx_) == tokenStartIdx_) {
                tokenStartIdx_ += commandLookup_[i].size();
                return Token(command_str_to_token_type(commandLookup_[i]));
            }
        }
        return Token(TokenType::UNKNOWN);
    }

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

    Token get_next_punctuation_token() {
        Token result(punctuation_str_to_token_type(command_.substr(tokenStartIdx_, 1)));
        ++tokenStartIdx_;
        return result;    
    }

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
            if (numArguments < num_function_arguments(command_str_to_token_type(commandLookup_[i]))) {
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
    const std::string validPunctuation_ = "(),=<>+-*/%^&|!";
};

} // namespace tokenizer

} // namespace kitty
