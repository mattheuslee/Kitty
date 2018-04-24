#pragma once

#include <kitty/stl_impl.hpp>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace kitty {

// Not using enum class due to int conversion requirement for ArduinoUnit
enum TokenType {
    CREATE_NUM, CREATE_LED, CREATE_SERVO,
    VAR_NAME, NUM_VAL, 
    UNKNOWN,
    CMD_END,
};

std::string token_type_to_str(TokenType tokenType) {
    switch (tokenType) {
    case TokenType::CREATE_NUM:
        return "CREATE_NUM";
    case TokenType::CREATE_LED:
        return "CREATE_LED";
    case TokenType::CREATE_SERVO:
        return "CREATE_SERVO";
    case TokenType::VAR_NAME:
        return "VAR_NAME";
    case TokenType::NUM_VAL:
        return "NUM_VAL";
    case TokenType::CMD_END:
        return "CMD_END";
    case UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType _type) {
        type = _type;
    }

    Token(TokenType _type, std::string _value) {
        type = _type;
        value = _value;
    }

    std::string str() const {
        return "Token(" + token_type_to_str(type) + ", " + value + ")";
    }
};

class Tokenizer {

public:
    Tokenizer(std::string command) : command_(command), command_stream_(command) {
    }

    Token get_next_token() {
        std::string word;
        command_stream_ >> word;
        if (word.size() == 0) {
            return Token(TokenType::CMD_END);
        }
        else if (isupper(word[0])) {
            return handle_command_token_(word);
        }
        else {
            return handle_regular_token_(word);
        }
    }

private:
    std::string command_;
    std::istringstream command_stream_;

    Token handle_command_token_(std::string word) {
        if (word == "CreateNumber") {
            return Token(TokenType::CREATE_NUM);
        }
        else if (word == "CreateLED") {
            return Token(TokenType::CREATE_LED);
        }
        else if (word == "CreateServo") {
            return Token(TokenType::CREATE_SERVO);
        }
        else {
            return handle_invalid_token_();
        }
    }

    Token handle_regular_token_(std::string word) {
        if (isalpha(word[0])) {
            return Token(TokenType::VAR_NAME, word);
        }
        return Token(TokenType::NUM_VAL, word);        
    }

    Token handle_invalid_token_() {
        return Token(TokenType::UNKNOWN);
    }

};

} // namespace kitty
