#pragma once

#include <kitty/stl_impl.hpp>
#include <stack>
#include <vector>

#include <kitty/tokenizer/tokenizer.hpp>
#include <kitty/utils/string_utils.hpp>

namespace kitty {

namespace parser {

class Parser {

public:
    Parser() {
    }
    
    explicit Parser(std::vector<kitty::tokenizer::Token> const & command) {
        set_command(command);
    }

    void set_command(std::vector<kitty::tokenizer::Token> const & command) {
        command_ = command;
        preprocess();
    }

    std::vector<kitty::tokenizer::Token> parse() {
        return run_shunting_yard();        
    }

    std::vector<kitty::tokenizer::Token> parse(std::vector<kitty::tokenizer::Token> const & command) {
        set_command(command);
        return parse();
    }

    void preprocess() {
        // Erase CMD_END token from the back if it exists
        if (command_[command_.size() - 1].type == tokenizer::TokenType::CMD_END) {
            command_.erase(command_.begin() + command_.size() - 1);
        }
        // Erase OP_PAREN token from the back if it exists
        if (command_[command_.size() - 1].type == tokenizer::TokenType::OP_PAREN) {
            command_.erase(command_.begin() + command_.size() - 1);
        }
    }

    std::vector<kitty::tokenizer::Token> run_shunting_yard() {
        std::stack<kitty::tokenizer::Token> operatorStack;
        std::vector<kitty::tokenizer::Token> output;
        
        for (int i = 0; i < command_.size(); ++i) {
            tokenizer::Token token = command_[i];
            if (tokenizer::is_operand(token)) {
                output.push_back(token);
            }
            else if (tokenizer::is_function(token)) {
                operatorStack.push(token);
            }
            else if (tokenizer::is_operator(token)) {
                while (!operatorStack.empty() &&
                       (tokenizer::is_function(operatorStack.top()) ||
                        tokenizer::has_greater_precedence(operatorStack.top(), token) ||
                           (tokenizer::has_equal_precedence(operatorStack.top(), token) && 
                            tokenizer::is_left_associative(operatorStack.top()))) &&
                       !tokenizer::is_left_bracket(operatorStack.top())) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();                    
                }
                operatorStack.push(token);
            }
            else if (tokenizer::is_left_bracket(token)) {
                operatorStack.push(token);
            }
            else if (tokenizer::is_right_bracket(token)) {
                while (!operatorStack.empty() && 
                       !is_left_bracket(operatorStack.top())) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();                    
                }
                operatorStack.pop();
            }
            else if (tokenizer::is_comma(token)) {
                while (!operatorStack.empty() && 
                       !is_left_bracket(operatorStack.top())) {
                    output.push_back(operatorStack.top());
                    operatorStack.pop();                    
                }
            }
        }
        while (!operatorStack.empty()) {
            output.push_back(operatorStack.top());
            operatorStack.pop();
        }
        return output;
    }

private:
    std::vector<kitty::tokenizer::Token> command_;

};

}

}
