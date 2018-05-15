#pragma once

#include <kty/stl_impl.hpp>
#include <stack>
#include <vector>

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/token.hpp>
#include <kty/tokenizer.hpp>
#include <kty/string_utils.hpp>
#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Class that performs parsing on commands.
*/
template <typename GetAllocFunc = decltype(get_alloc), typename GetPoolFunc = decltype(get_stringpool), typename Token = Token<>, typename PoolString = PoolString<>>
class Parser {

public:
    /*!
        @brief  Constructor for Parser object.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    Parser(GetAllocFunc & getAllocFunc = get_alloc, GetPoolFunc & getPoolFunc = get_stringpool)
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc), command_(getAllocFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
    }
    
    /*!
        @brief  Constructor for Parser object.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.

        @param  command
                The tokenized command to parse.
    */
    Parser(GetAllocFunc & getAllocFunc, GetPoolFunc & getPoolFunc, Deque<Token> const & command)
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc), command_(getAllocFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        set_command(command);
    }

    /*!
        @brief  Sets the command for the parser to parse.

        @param  command
                The tokenized command to parse.
    */
    void set_command(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        command_ = command;
        preprocess();
    }

    /*!
        @brief  Parses the command stored in the parser.

        @return The parsed command tokens.
    */
    Deque<Token> parse() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return run_shunting_yard();        
    }

    /*!
        @brief  Parses the given command.

        @param  command
                The tokenized command to parse.

        @return The parsed command tokens.
    */
    Deque<Token> parse(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        set_command(command);
        return parse();
    }

    /*!
        @brief  Preprocesses the stored command to prepare for parsing.
    */
    void preprocess() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Erase CMD_END token from the back if it exists
        if (command_.size() > 0 && command_.back().is_cmd_end()) {
            command_.pop_back();
        }
        // Erase OP_PAREN token from the back if it exists
        if (command_.size() > 0 && command_.back().is_op_paren()) {
            command_.pop_back();
        }
    }

     /*!
        @brief  Runs the shunting yard algorithm on the stored command.
                The algorithm converts an infix expression to a postfix expression.
        
        @return The converted postfix expression.
    */
    Deque<Token> run_shunting_yard() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> operatorStack(*getAllocFunc_);
        Deque<Token> output(*getAllocFunc_);

        for (typename Deque<Token>::Iterator it = command_.begin(); it != command_.end(); ++it) {
            Token & token = *it;
            if (token.is_operand()) {
                Log.verbose(F("%s: operand\n"), PRINT_FUNC);
                Log.verbose(F("%s: operand %s pushed to output\n"), PRINT_FUNC, token.str().c_str());
                output.push_back(token);
            }
            else if (token.is_function()) {
                Log.verbose(F("%s: function\n"), PRINT_FUNC);
                Log.verbose(F("%s: function %s pushed to operator stack\n"), PRINT_FUNC, token.str().c_str());
                operatorStack.push_back(token);
            }
            else if (token.is_operator()) {
                Log.verbose(F("%s: operator\n"), PRINT_FUNC);
                while (!operatorStack.is_empty() &&
                       (operatorStack.back().is_function() ||
                        operatorStack.back().has_greater_precedence_than(token) ||
                           (operatorStack.back().has_equal_precedence_to(token) && 
                            operatorStack.back().is_left_associative())) &&
                       !operatorStack.back().is_op_paren()) {
                    Log.verbose(F("%s: operator %s pushed from operator stack to output\n"), PRINT_FUNC, operatorStack.back().str().c_str());
                    output.push_back(operatorStack.back());
                    operatorStack.pop_back();                    
                }
                Log.verbose(F("%s: operator %s pushed to operator stack\n"), PRINT_FUNC, token.str().c_str());
                operatorStack.push_back(token);
            }
            else if (token.is_op_paren()) {
                Log.verbose(F("%s: op_paren\n"), PRINT_FUNC);
                Log.verbose(F("%s: op paren %s pushed to operator stack\n"), PRINT_FUNC, token.str().c_str());
                operatorStack.push_back(token);
            }
            else if (token.is_cl_paren()) {
                Log.verbose(F("%s: cl_paren\n"), PRINT_FUNC);
                while (!operatorStack.is_empty() && 
                       !operatorStack.back().is_op_paren()) {
                    Log.verbose(F("%s: %s pushed from operator stack to output\n"), PRINT_FUNC, operatorStack.back().str().c_str());
                    output.push_back(operatorStack.back());
                    operatorStack.pop_back();                    
                }
                Log.verbose(F("%s: %s popped from operator stack\n"), PRINT_FUNC, operatorStack.back().str().c_str());
                operatorStack.pop_back();
            }
            else if (token.is_comma()) {
                Log.verbose(F("%s: comma\n"), PRINT_FUNC);
                while (!operatorStack.is_empty() && 
                       !operatorStack.back().is_op_paren()) {
                    Log.verbose(F("%s: %s pushed from operator stack to output\n"), PRINT_FUNC, operatorStack.back().str().c_str());
                    output.push_back(operatorStack.back());
                    operatorStack.pop_back();                    
                }
            }
            else {
                Log.verbose(F("%s: others\n"), PRINT_FUNC);
                Log.verbose(F("%s: %s pushed to output\n"), PRINT_FUNC, token.str().c_str());
                output.push_back(token);
            }
        }
        while (!operatorStack.is_empty()) {
            Log.verbose(F("%s: %s pushed from operator stack to output\n"), PRINT_FUNC, operatorStack.back().str().c_str());
            output.push_back(operatorStack.back());
            operatorStack.pop_back();
        }
        return output;
    }

private:
    GetAllocFunc * getAllocFunc_;
    GetPoolFunc * getPoolFunc_;
    Deque<Token> command_;

};

} // namespace kty
