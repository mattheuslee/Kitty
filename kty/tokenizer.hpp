#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

#include <algorithm>
#include <cctype>
#include <sstream>

#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/string_utils.hpp>
#include <kty/token.hpp>

namespace kty {

/*!
    @brief  Class that tokenizes commands.
*/
template <typename Alloc, typename StringPool>
class Tokenizer {

public:

    /*!
        @brief  Constructor for tokenizer.

        @param  alloc
                The allocator to use.

        @param  stringPool
                The string pool to use.
    */
    explicit Tokenizer(Alloc & alloc, StringPool & stringPool) 
        : alloc_(alloc), stringPool_(stringPool), command_(stringPool),
          validPunctuation_(stringPool, "(),=<>+-*/%^&|!~") {
    }

    /*!
        @brief  Constructor for tokenizer that takes in command to tokenize.

        @param  alloc
                The allocator to use.

        @param  stringPool
                The string pool to use.

        @param  command
                The command to tokenize.
    */
    explicit Tokenizer(Alloc & alloc, StringPool & stringPool, PoolString<StringPool> const & command) 
        : alloc_(alloc), stringPool_(stringPool), command_(stringPool),
          validPunctuation_(stringPool, "(),=<>+-*/%^&|!~") {
        set_command(command);
    }

    /*!
        @brief  Sets the command to be tokenized.

        @param  command
                The command to tokenize.
    */
    void set_command(PoolString<StringPool> const & command) {
        command_ = command;
        remove_str_whitespace(command_);
        add_missing_optional_arguments();
        tokenStartIdx_ = 0;
    }

    /*!
        @brief  Tokenizes the command stored in the tokenizer.

        @return The tokenized command.
    */
    Deque<Token<StringPool>, Alloc> tokenize() {
        Deque<Token<StringPool>, Alloc> tokens(alloc_);
        Token<StringPool> token(TokenType::UNKNOWN_TOKEN, stringPool_);
        do {
            token = get_next_token();
            if (!token.is_unknown_token_type()) {
                tokens.push_back(token);
            }
        } while (!token.is_cmd_end());
        process_math_tokens(tokens);
        return tokens;
    }

    /*!
        @brief  Tokenizes the given command.

        @param  command
                The command to tokenize.

        @return The tokenized command.
    */
    Deque<Token<StringPool>, Alloc> tokenize(PoolString<StringPool> const & command) {
        set_command(command);
        return tokenize();
    }

    /*!
        @brief  Performs processing on the math tokens within the command.
                Comparison tokens are compressed, and unary '-' tokens are detected.

        @param  tokens
                The tokenized command.
    */
    void process_math_tokens(Deque<Token<StringPool>, Alloc> & tokens) {
        // Compresses '<' + '=' into '<=' and '>' + '=' into '>='
        typename Deque<Token<StringPool>, Alloc>::Iterator iter = tokens.begin();
        while (iter != tokens.end()) {
            typename Deque<Token<StringPool>, Alloc>::Iterator next = iter;
            ++next;
            if (next != tokens.end()) {
                if (iter->is_less() && next->is_equals()) {
                    iter->set_type(TokenType::L_EQUALS);
                    next = tokens.erase(next);
                }
                else if (iter->is_greater() && next->is_equals()) {
                    iter->set_type(TokenType::G_EQUALS);
                    next = tokens.erase(next);
                }
            }
            iter = next;
        }

        // Detects unary '-' tokens
        for (iter = tokens.begin(); iter != tokens.end(); ++iter) {
            if (iter->is_math_sub()) {
                typename Deque<Token<StringPool>, Alloc>::Iterator prev = iter;
                --prev;
                if (iter == tokens.begin() ||
                    prev->is_operator() ||
                    prev->is_op_paren()) {
                    iter->set_type(TokenType::UNARY_NEG);
                }
            }
        }
    }

    /*!
        @brief  Gets the next token from command stored in the tokenizer.

        @return The next token from the stored command.
    */
    Token<StringPool> get_next_token() {
        // No more tokens
        if (tokenStartIdx_ == command_.strlen()) {
            return Token<StringPool>(TokenType::CMD_END, stringPool_);
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
        else if (validPunctuation_.find(command_[tokenStartIdx_]) != -1) {
            return get_next_punctuation_token();            
        }
        return Token<StringPool>(TokenType::UNKNOWN_TOKEN, stringPool_);
    }

    /*!
        @brief  Gets the next command token from the stored command.

        @return The next command token.
                If the next token is not a command, an unknown token is returned.
    */
    Token<StringPool> get_next_command_token() {
        Token<StringPool> token(TokenType::UNKNOWN_TOKEN, stringPool_);
        for (int i = 0; i < commandLookupSize_; ++i) {
            if (command_.find(get_command_lookup()[i], tokenStartIdx_) == tokenStartIdx_) {
                tokenStartIdx_ += ::strlen(get_command_lookup()[i]);
                token.set_type(command_str_to_token_type(get_command_lookup()[i]));
                return token;
            }
        }
        return token;
    }

    /*!
        @brief  Gets the next name token from the stored command.

        @return The next name token.
                If the next token is not a name, an unknown token is returned.
    */
    Token<StringPool> get_next_name_token() {
        int currIdx = tokenStartIdx_;
        while (currIdx < command_.strlen() && (islower(command_[currIdx]) || command_[currIdx] == '_')) {
            ++currIdx;
        }
        PoolString<StringPool> value = command_.substr_i(tokenStartIdx_, currIdx);
        Token<StringPool> result(TokenType::NAME, stringPool_, value);
        tokenStartIdx_ = currIdx;
        return result;
    }

    /*!
        @brief  Gets the next number token from the stored command.

        @return The next number token.
                If the next token is not a number, an unknown token is returned.
    */
    Token<StringPool> get_next_number_token() {
        int currIdx = tokenStartIdx_;
        while (currIdx < command_.strlen() && isdigit(command_[currIdx])) {
            ++currIdx;
        }
        PoolString<StringPool> value = command_.substr_i(tokenStartIdx_, currIdx);
        Token<StringPool> result(TokenType::NUM_VAL, stringPool_, value);
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
    Token<StringPool> get_next_string_token(char const & open) {
        int endIdx = command_.find(open, tokenStartIdx_ + 1);
        // Only take substr of the string, without quotes
        Token<StringPool> result(TokenType::STRING, command_.substr(tokenStartIdx_ + 1, endIdx - tokenStartIdx_ - 1));
        tokenStartIdx_ = endIdx + 1;
        return result;
    }

    /*!
        @brief  Gets the next punctuation token from the stored command.

        @return The next punctuation token.
                If the next token is not punctuation, an unknown token is returned.
    */
    Token<StringPool> get_next_punctuation_token() {
        Token<StringPool> result(punctuation_char_to_token_type(command_[tokenStartIdx_]), stringPool_);
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
    PoolString<StringPool> get_additional_arguments(TokenType tokenType, int const & numArguments) {
        PoolString<StringPool> arguments(stringPool_);
        switch (tokenType) {
        case TokenType::CREATE_NUM:
            arguments = "0";
            break;
        case TokenType::CREATE_LED:
            arguments = ",50";
            break;
        case TokenType::CREATE_GROUP:
            arguments = "";
            break;
        case TokenType::RUN_GROUP:
            arguments = "01";
            break;
        case TokenType::MOVE_BY_FOR:
            arguments = "";
            break;
        case TokenType::MOVE_BY:
            arguments = "";
            break;
        case TokenType::SET_TO_FOR:
            arguments = "";
            break;
        case TokenType::SET_TO:
            arguments = "";
            break;
        case TokenType::IF:
            arguments = "";
            break;
        case TokenType::ELSE:
            arguments = "";
            break;
        }
        return arguments;
    }

    /*!
        @brief  Adds the missing arguments, if any, for the functions present in the command.
    */
    void add_missing_optional_arguments() {
        for (int i = 0; i < commandLookupSize_; ++i) {
            int idx = command_.find(get_command_lookup()[i]);
            if (idx == -1) {
                continue;
            }
            int opParenIdx = command_.find("(", idx + ::strlen(get_command_lookup()[i]));
            int clParenIdx = command_.find(')', opParenIdx);
            int numArguments = 0;
            // At least one argument between them
            if (clParenIdx > opParenIdx + 1) {
                numArguments = command_.count(',', opParenIdx, clParenIdx) + 1;
            }
            // Need to fill up arguments
            TokenType tokenType = command_str_to_token_type(get_command_lookup()[i]);
            int requiredArguments = Token<StringPool>(tokenType, stringPool_).num_function_arguments();
            if (numArguments < requiredArguments) {
                PoolString<StringPool> additionalArguments = get_additional_arguments(tokenType, numArguments);
                command_insert(additionalArguments, clParenIdx);
            }
        }
    }

    /*!
        @brief  Get a 2D array containing all command words.

        @return A pointer to a 2D character array containing all 
                command words.
    */
    char const ** get_command_lookup() const {
        static const char commandLookup[][10] = {
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
        return commandLookup;
    }

private:
    StringPool & stringPool_;
    Alloc & alloc_;

    PoolString<StringPool> command_;
    int tokenStartIdx_ = 0;

    const int commandLookupSize_ = 13;
    PoolString<StringPool> validPunctuation_;
};

} // namespace kty
