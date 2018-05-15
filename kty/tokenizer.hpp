#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

#include <algorithm>
#include <cctype>
#include <sstream>

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/string_utils.hpp>
#include <kty/token.hpp>
#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Class that tokenizes commands.
*/
template <typename GetAllocFunc = decltype(get_alloc), typename GetPoolFunc = decltype(get_stringpool), typename Token = Token<>, typename PoolString = PoolString<>>
class Tokenizer {

public:

    /*!
        @brief  Constructor for tokenizer.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    Tokenizer(GetAllocFunc & getAllocFunc = get_alloc, GetPoolFunc & getPoolFunc = get_stringpool) 
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc), command_(getPoolFunc),
          validPunctuation_(get_stringpool, "(),=<>+-*/%^&|!~") {
        Log.verbose(F("%s\n"), PRINT_FUNC);
    }

    /*!
        @brief  Constructor for tokenizer that takes in command to tokenize.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.

        @param  command
                The command to tokenize.
    */
    Tokenizer(GetAllocFunc & getAllocFunc, GetPoolFunc & getPoolFunc, PoolString const & command) 
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc), command_(getPoolFunc),
          validPunctuation_(getPoolFunc, "(),=<>+-*/%^&|!~") {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        set_command(command);
    }

    /*!
        @brief  Sets the command to be tokenized.

        @param  command
                The command to tokenize.
    */
    void set_command(PoolString const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        command_ = command;
        remove_str_whitespace(command_);
        add_missing_optional_arguments();
        tokenStartIdx_ = 0;
    }

    /*!
        @brief  Tokenizes the command stored in the tokenizer.

        @return The tokenized command.
    */
    Deque<Token> tokenize() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokens(*getAllocFunc_);
        tokens.clear();
        Token token(TokenType::UNKNOWN_TOKEN, *getPoolFunc_);
        do {
            token = get_next_token();
            Log.verbose(F("%s: next token is %s\n"), PRINT_FUNC, token.str().c_str());
            if (!token.is_unknown_token()) {
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
    Deque<Token> tokenize(PoolString const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        set_command(command);
        return tokenize();
    }

    /*!
        @brief  Performs processing on the math tokens within the command.
                Comparison tokens are compressed, and unary '-' tokens are detected.

        @param  tokens
                The tokenized command.
    */
    void process_math_tokens(Deque<Token> & tokens) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Compresses '<' + '=' into '<=' and '>' + '=' into '>='
        typename Deque<Token>::Iterator iter = tokens.begin();
        while (iter != tokens.end()) {
            typename Deque<Token>::Iterator next = iter;
            ++next;
            if (next != tokens.end()) {
                if (iter->is_less() && next->is_equals()) {
                    Log.verbose(F("%s: compressing into <=\n"), PRINT_FUNC);
                    iter->set_type(TokenType::L_EQUALS);
                    next = tokens.erase(next);
                }
                else if (iter->is_greater() && next->is_equals()) {
                    Log.verbose(F("%s: compressing into >=\n"), PRINT_FUNC);
                    iter->set_type(TokenType::G_EQUALS);
                    next = tokens.erase(next);
                }
            }
            iter = next;
        }

        // Detects unary '-' tokens
        for (iter = tokens.begin(); iter != tokens.end(); ++iter) {
            if (iter->is_math_sub()) {
                typename Deque<Token>::Iterator prev = iter;
                --prev;
                if (iter == tokens.begin() ||
                    prev->is_operator() ||
                    prev->is_op_paren()) {
                    Log.verbose(F("%s: changing to unary -\n"), PRINT_FUNC);
                    iter->set_type(TokenType::UNARY_NEG);
                }
            }
        }
    }

    /*!
        @brief  Gets the next token from command stored in the tokenizer.

        @return The next token from the stored command.
    */
    Token get_next_token() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // No more tokens
        if (tokenStartIdx_ >= command_.strlen()) {
            return Token(TokenType::CMD_END, *getPoolFunc_);
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
        ++tokenStartIdx_;
        Log.warning(F("%s: unknown token %c\n"), PRINT_FUNC, command_[tokenStartIdx_ - 1]);
        return Token(TokenType::UNKNOWN_TOKEN, *getPoolFunc_);
    }

    /*!
        @brief  Gets the next command token from the stored command.

        @return The next command token.
                If the next token is not a command, an unknown token is returned.
    */
    Token get_next_command_token() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Token token(TokenType::UNKNOWN_TOKEN, *getPoolFunc_);
        for (int i = 0; i < commandLookupSize_; ++i) {
            if (command_.find(get_command_lookup()[i], tokenStartIdx_) == tokenStartIdx_) {
                tokenStartIdx_ += ::strlen(get_command_lookup()[i]);
                token.set_type(command_str_to_token_type(get_command_lookup()[i]));
                return token;
            }
        }
        Log.warning(F("%s: not a valid command\n"), PRINT_FUNC);
        // Not a valid command,
        // Skip forward until the end of this word
        for (++tokenStartIdx_; tokenStartIdx_ < command_.strlen() && islower(command_[tokenStartIdx_]); ++tokenStartIdx_);
        return token;
    }

    /*!
        @brief  Gets the next name token from the stored command.

        @return The next name token.
                If the next token is not a name, an unknown token is returned.
    */
    Token get_next_name_token() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int currIdx = tokenStartIdx_;
        while (currIdx < command_.strlen() && (islower(command_[currIdx]) || command_[currIdx] == '_')) {
            ++currIdx;
        }
        Token result(TokenType::NAME, command_.substr_ii(tokenStartIdx_, currIdx));
        tokenStartIdx_ = currIdx;
        return result;
    }

    /*!
        @brief  Gets the next number token from the stored command.

        @return The next number token.
                If the next token is not a number, an unknown token is returned.
    */
    Token get_next_number_token() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int currIdx = tokenStartIdx_;
        while (currIdx < command_.strlen() && isdigit(command_[currIdx])) {
            ++currIdx;
        }
        Token result(TokenType::NUM_VAL, command_.substr_ii(tokenStartIdx_, currIdx));
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
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int endIdx = command_.find(open, tokenStartIdx_ + 1);
        // Only take substr of the string, without quotes
        Token result(TokenType::STRING, command_.substr_ii(tokenStartIdx_ + 1, endIdx));
        tokenStartIdx_ = endIdx + 1;
        return result;
    }

    /*!
        @brief  Gets the next punctuation token from the stored command.

        @return The next punctuation token.
                If the next token is not punctuation, an unknown token is returned.
    */
    Token get_next_punctuation_token() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Token result(punctuation_char_to_token_type(command_[tokenStartIdx_]), *getPoolFunc_);
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
    PoolString get_additional_arguments(TokenType tokenType, int const & numArguments) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        PoolString arguments(*getPoolFunc_);
        switch (tokenType) {
        case TokenType::CREATE_NUM:
            if (numArguments < 1) {
                arguments += "0";
            }
            break;
        case TokenType::CREATE_LED:
            if (numArguments < 1) {
                arguments += "13";
            }
            if (numArguments < 2) {
                arguments += ",50";
            }
            break;
        case TokenType::RUN_GROUP:
            if (numArguments < 1) {
                arguments += "1";
            }
            break;
        }
        return arguments;
    }

    /*!
        @brief  Adds the missing arguments, if any, for the functions present in the command.
    */
    void add_missing_optional_arguments() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
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
            int requiredArguments = Token(tokenType, *getPoolFunc_).num_function_arguments();
            if (numArguments < requiredArguments) {
                PoolString additionalArguments(get_additional_arguments(tokenType, numArguments));
                command_.insert(additionalArguments.c_str(), clParenIdx);
            }
        }
    }

    /*!
        @brief  Get a 2D array containing all command words.

        @return A pointer to a 2D character array containing all 
                command words.
    */
    char const (*get_command_lookup(void) const)[10] {
    //char const *[10] get_command_lookup() const {
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
    GetAllocFunc * getAllocFunc_;
    GetPoolFunc * getPoolFunc_;

    PoolString command_;
    int tokenStartIdx_ = 0;

    const int commandLookupSize_ = 13;
    PoolString validPunctuation_;
};

} // namespace kty
