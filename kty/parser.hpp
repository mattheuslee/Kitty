#pragma once

#include <kty/stl_impl.hpp>
#include <stack>
#include <vector>

#include <kty/tokenizer.hpp>
#include <kty/string_utils.hpp>

namespace kty {

/*!
    @brief  Class that performs parsing on commands.
*/
class Parser {

public:

    /*!
        @brief Default constructor for Parser object.
    */
    Parser() {
    }
    
    /*!
        @brief  Constructor for Parser object.

        @param  command
                The tokenized command to parse.
    */
    explicit Parser(std::vector<Token> const & command) {
        set_command(command);
    }

    /*!
        @brief  Sets the command for the parser to parse.

        @param  command
                The tokenized command to parse.
    */
    void set_command(std::vector<Token> const & command) {
        command_ = command;
        preprocess();
    }

    /*!
        @brief  Parses the command stored in the parser.

        @return The parsed command tokens.
    */
    std::vector<Token> parse() {
        return run_shunting_yard();        
    }

    /*!
        @brief  Parses the given command.

        @param  command
                The tokenized command to parse.

        @return The parsed command tokens.
    */
    std::vector<Token> parse(std::vector<Token> const & command) {
        set_command(command);
        return parse();
    }

    /*!
        @brief  Preprocesses the stored command to prepare for parsing.
    */
    void preprocess() {
        // Erase CMD_END token from the back if it exists
        if (command_[command_.size() - 1].type == TokenType::CMD_END) {
            command_.erase(command_.begin() + command_.size() - 1);
        }
        // Erase OP_PAREN token from the back if it exists
        if (command_[command_.size() - 1].type == TokenType::OP_PAREN) {
            command_.erase(command_.begin() + command_.size() - 1);
        }
    }

     /*!
        @brief  Runs the shunting yard algorithm on the stored command.
                The algorithm converts an infix expression to a postfix expression.
        
        @return The converted postfix expression.
    */
    std::vector<Token> run_shunting_yard() {
        std::stack<Token> operatorStack;
        std::vector<Token> output;
        
        for (int i = 0; i < command_.size(); ++i) {
            Token token = command_[i];
            if (token.is_operand()) {
                output.push_back(token);
            }
            else if (token.is_function()) {
                operatorStack.push(token);
            }
            else if (token.is_operator()) {
                while (!operatorStack.empty() &&
                       (operatorStack.top().is_function() ||
                        operatorStack.top().has_greater_precedence_than(token) ||
                           (operatorStack.top().has_equal_precedence_to(token) && 
                            operatorStack.top().is_left_associative())) &&
                       !operatorStack.top().is_left_bracket()) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();                    
                }
                operatorStack.push(token);
            }
            else if (token.is_left_bracket()) {
                operatorStack.push(token);
            }
            else if (token.is_right_bracket()) {
                while (!operatorStack.empty() && 
                       !operatorStack.top().is_left_bracket()) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();                    
                }
                operatorStack.pop();
            }
            else if (token.is_comma()) {
                while (!operatorStack.empty() && 
                       !operatorStack.top().is_left_bracket()) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();                    
                }
            }
            else {
                output.push_back(token);
            }
        }
        while (!operatorStack.empty()) {
            output.push_back(operatorStack.top());
            operatorStack.pop();
        }
        return output;
    }

private:
    std::vector<kty::Token> command_;

};

} // namespace kty
