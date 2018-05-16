#pragma once

#if defined(ARDUINO)
#include <Servo.h>
#endif

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/machine_state.hpp>
#include <kty/parser.hpp>
#include <kty/string_utils.hpp>
#include <kty/token.hpp>
#include <kty/tokenizer.hpp>
#include <kty/types.hpp>

namespace kty {

/** The various status types of the interpreter */
enum InterpreterStatus {
    CREATING_IF,
    CREATING_ELSE,
    CREATING_WHILE,
    CREATING_GROUP,
    NORMAL,
};

/*!
    @brief  Class that stores state on all devices and groups, and executes commands.
*/
template <typename GetAllocFunc = decltype(get_alloc), typename GetPoolFunc = decltype(get_stringpool), typename PoolString = PoolString<>, typename Token = Token<>>
class Interpreter {

public:
    /*!
        @brief  Default interpreter constructor.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    Interpreter(GetAllocFunc & getAllocFunc = get_alloc, GetPoolFunc & getPoolFunc = get_stringpool)
            : getAllocFunc_(getAllocFunc), getPoolFunc_(getPoolFunc),
              commandQueue_(getAllocFunc), commandBuffer_(getAllocFunc),
              machineState_(getAllocFunc, getPoolFunc),
              lastGroupName_(getPoolFunc),
              lastCondition_(getAllocFunc),
              parser_(getAllocFunc, getPoolFunc), tokenizer_(getAllocFunc, getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        status_ = InterpreterStatus::NORMAL;
        currScopeLevel_ = 0;          // Start at scope level 0
        lastCondition_.push_back(-1); // Last condition at scope level 0 = null
        bracketParity_ = 0;
    }

    /*!
        @brief  Resets the internal interpreter state and memory.
                Essentially starting the interpreter from the beginning.
    */
    void reset() {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        status_ = InterpreterStatus::NORMAL;
        currScopeLevel_ = 0;
        lastCondition_[currScopeLevel_] = -1;
        bracketParity_ = 0;
        lastGroupName_ = "";
        machineState_.reset();
        commandQueue_.clear();
        commandBuffer_.clear();
    }

    /*!
        @brief  Gets the prefix(if any) for the user prompt.

        @return The prefix for the user prompt.
                If there is no required prefix, an empty string is returned.
    */
    PoolString get_prompt_prefix() {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        PoolString prefix(*getPoolFunc_);
        switch (status_) {
        case NORMAL:
            prefix = "";
            break;
        case CREATING_GROUP:
            prefix = "(";
            prefix += lastGroupName_;
            prefix += ") ";
            break;
        case CREATING_IF:
            prefix = "(IF) ";
            break;
        case CREATING_ELSE:
            prefix = "(ELSE) ";
            break;
        }
        return prefix;
    }

    /*!
        @brief  Checks if a number with the given name exists.
        
        @param  name
                The name of the number.
        
        @return True if the number exists, false otherwise.
    */
    bool number_exists(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        return machineState_.number_exists(name);
    }

    /*!
        @brief  Checks whether a device exists.

        @param  name
                The name of the device to search for.

        @return True if the device exists, false otherwise.
    */
    bool device_exists(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        return machineState_.device_exists(name);
    }

    /*!
        @brief  Checks whether a group exists.

        @param  name
                The name of the group to search for.

        @return True if the group exists, false otherwise.
    */
    int group_exists(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        return machineState_.group_exists(name);
    }

    /*!
        @brief  Gets the value of a number.

        @param  name
                The number name.
        
        @return The value of the number.
                Returns 0 if it does not exist.
    */
    int get_number_value(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        return machineState_.get_number_value(name);        
    }

    /*!
        @brief  Gets the type of device.

        @param  name
                The device name.
        
        @return The type of the device.
                Returns an unknown device if it does not exist.
    */
    DeviceType get_device_type(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        return machineState_.get_device_type(name);        
    }

    /*!
        @brief  Gets the info of device.

        @param  name
                The device name.

        @param  idx
                The info index.
        
        @return The info of the device.
                Returns -1 if it does not exist.
    */
    int get_device_info(PoolString const & name, int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return machineState_.get_device_info(name, idx);
    }

    /*!
        @brief  Gets the commands within a group.

        @param  name
                The name of the group.

        @return The commands within the group.
                If the group does not exist, an empty deque is returned.
    */
    Deque<PoolString> get_group_commands(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        return machineState_.get_group_commands(name);        
    }

    /*!
        @brief  Executes a given command, as well as the commands in the command
                queue afterwards if necessary.
                This should be the command execute function called from outside.

        @param  command
                The command to execute.
    */
    void execute(PoolString command) {
        remove_str_multiple_whitespace(command);
        commandQueue_.push_back(command);
        execute_command_queue();
    }

    /*!
        @brief  Executes just the given command. 
                This method takes the current interpreter status into account.
                If the result of this command is to add other commands to the 
                command queue, those commands will not be run yet.
                execute_command_queue() must be called after this to run those
                commands.

        @param  command
                The command to execute.
    */
    void execute_single_command(PoolString const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Nothing to execute
        if (command.strlen() == 0) {
            return;
        }
        // Check for special interpreter-only commands
        if (command == "DecreaseScopeLevel") {
            --currScopeLevel_;
            return;
        }
        Deque<Token> tokens;
        switch (status_) {
        case NORMAL:
            tokens = tokenizer_.tokenize(command);
            tokens = parser_.parse(tokens);
            execute_command_tokens(tokens);
            break;
        case CREATING_IF:
            add_to_if(command);
            break;
        case CREATING_ELSE:
            add_to_else(command);
            break;
        case CREATING_GROUP:
            add_to_group(command);
            break;
        };
    }

    /*!
        @brief  Executes all the commands still in the command queue, if any.
    */
    void execute_command_queue() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        while (!commandQueue_.is_empty()) {
            PoolString command(commandQueue_.front());
            commandQueue_.pop_front();
            execute_single_command(command);
        }
    }

    /*!
        @brief  Executes a given command in the form of tokens.

        @param  command
                The command to execute.
                Tokens in command are assumed to be in postfix notation.
    */
    void execute_command_tokens(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (command.back().is_if()) {
            execute_if(command);
        }
        else if (command.back().is_else()) {
            execute_else(command);
        }
        else {
            // For every other command, last condition at this scope level becomes null
            lastCondition_[currScopeLevel_] = -1;
            if (command.front().is_name()) {
                // Printing information
                if (command.size() == 1) {
                    execute_print_info(command);
                }
                // Creation of a device/group
                else if (command.back().is_create_num() || 
                         command.back().is_create_led() || 
                         command.back().is_create_group()) {
                    execute_create(command);
                }
                else if (command.back().is_move_by() ||
                         command.back().is_move_by_for()) {
                    execute_move_by(command);
                }
                // Running group
                else {
                    execute_run_group(command);
                }
            }
            else if (command.front().is_string()) {
                execute_print_string(command);
            }
        }
    }

    /*!
        @brief  Executes the print information command.

        @param  command
                The command to execute.
    */
    void execute_print_info(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        PoolString name(command.front().get_value());
        if (number_exists(name)) {
            Serial.print(name.c_str());
            Serial.print(F(": number storing "));
            Serial.println(get_number_value(name));
        }
        else if (device_exists(name)) {
            switch (get_device_type(name)) {
            case LED:
                Serial.print(name.c_str());
                Serial.print(F(": LED using pin "));
                Serial.print(get_device_info(name, 1));
                Serial.print(F(" at "));
                Serial.print(get_device_info(name, 2));
                Serial.println(F("%"));                
            };
        }
        else if (group_exists(name)) {
            Serial.print(name.c_str());
            Serial.println(F(": group containing the command(s) "));
            int nameLen = name.strlen();
            Deque<PoolString> groupCommands = get_group_commands(name);
            for (typename Deque<PoolString>::Iterator it = groupCommands.begin(); it != groupCommands.end(); ++it) {
                // Print out enough spaces to line up vertically with the end of
                // the name of the group
                for (int j = 0; j < nameLen; ++j) {
                    Serial.print(F(" "));
                }
                Serial.println(it->c_str());
            }
        }
        else {
            Serial.print(F("Error: "));
            Serial.print(name.c_str());
            Serial.println(F(" does not exist"));
        }
    }

    /*!
        @brief  Executes the if command.

        @param  command
                The command to execute.
    */
    void execute_if(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokenQueue(command);
        // Skip the if token at the end
        tokenQueue.pop_back();
        Deque<Token> result = evaluate_postfix(tokenQueue);
        create_if(get_token_value(result.back()));
    }

    /*!
        @brief  Executes the else command.

        @param  command
                The command to execute.
    */
    void execute_else(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // No condition to extract for else, immediately create group
        create_else();
    }

    /*!
        @brief  Executes the different creation commands.

        @param  command
                The command to execute.
    */
    void execute_create(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokenQueue(command);
        // Skip the create token at the end
        Token createToken = tokenQueue.back();
        tokenQueue.pop_back();
        PoolString name(tokenQueue.front().get_value());
        tokenQueue.pop_front();

        Deque<Token> result = evaluate_postfix(tokenQueue);

        if (createToken.is_create_num()) {
            create_number(name, result);
        }
        else if (createToken.is_create_led()) {
            create_led(name, result);
        }
        else if (createToken.is_create_group()) {
            create_group(name);
        }
    }

    /*!
        @brief  Executes the move by or move by for commands.

        @param  command
                The command to execute.
    */
    void execute_move_by(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokenQueue(command);
        Token moveByToken = tokenQueue.back();
        tokenQueue.pop_back();
        PoolString name(tokenQueue.front().get_value());
        tokenQueue.pop_front();
        // Nothing to move
        if (!number_exists(name) && !device_exists(name)) {
            Serial.print(F("Error: "));
            Serial.print(name.c_str());
            Serial.println(F(" does not exist"));
            return;
        }

        Deque<Token> result = evaluate_postfix(tokenQueue);

        int displacement, durationMs = 0;
        // There will be an additional argument if it is move by for
        if (moveByToken.is_move_by_for()) {
            durationMs = get_token_value(result.back());
            result.pop_back();
        }
        displacement = get_token_value(result.back());

        // Execute move
        int value = get_number_value(name);
        int deviceInfo1 = get_device_info(name, 1);
        int deviceInfo2 = get_device_info(name, 2);

        if (number_exists(name)) {
            machineState_.set_number(name, value + displacement);
        }
        else {
            switch (get_device_type(name)) {
            case LED:
                int brightness = deviceInfo2 + displacement;
                if (brightness > 100) {
                    Log.notice(F("%s: LED brightness over 100%, maximum is 100%\n"), PRINT_FUNC);
                    brightness = 100;
                }
                else if (brightness < 0) {
                    Log.notice(F("%s: LED brightness below 0%, minimum is 0%\n"), PRINT_FUNC);
                    brightness = 0;
                }
                analogWrite(deviceInfo1, brightness * 2.55);
                machineState_.set_device(name, DeviceType::LED, -1, deviceInfo1, brightness);
            };
        }

        if (command.back().is_move_by_for()) {
            // Additional time delay
            delay(durationMs);
            // Then set back to original value
            if (number_exists(name)) {
                machineState_.set_number(name, value);
            }
            else {
                switch (get_device_type(name)) {
                case LED:
                    analogWrite(deviceInfo1, deviceInfo2 * 2.55);
                    machineState_.set_device(name, DeviceType::LED, -1, deviceInfo1, deviceInfo2);
                };
            }
        }
    }

    /*!
        @brief  Executes the running of a command group.
                Essentially adds the commands in the command group to the
                command queue to be executed later.

        @param  command
                The command to execute.
    */
    void execute_run_group(Deque<Token> const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokenQueue(command);
        // Remove RunGroup command from back
        tokenQueue.pop_back();
        // Extract name of group and check if it exists
        PoolString name(tokenQueue.front().get_value());
        if (!group_exists(name)) {
            Log.warning(F("%s: %s does not exist\n"), PRINT_FUNC, name.c_str());
            return;
        }
        // Extract number of times to run group
        Deque<Token> result = evaluate_postfix(tokenQueue);
        int numTimes = get_token_value(result.back());
        // Push command for one more call to run the group
        if (numTimes > 1) {
            commandQueue_.push_front(PoolString(*getPoolFunc_));
            commandQueue_.front() += name.c_str();
            commandQueue_.front() += "RunGroup(";
            commandQueue_.front() += int_to_str(numTimes - 1, *getPoolFunc_);
            commandQueue_.front() += ")";
        }
        // Continuously run the group
        else if (numTimes == -1) {
            commandQueue_.push_front(PoolString(*getPoolFunc_));
            commandQueue_.front() += name.c_str();
            commandQueue_.front() += "RunGroup(-1)";
        }
        // If running group at least once(or continuously)
        if (numTimes == -1 || numTimes > 0) {
            Deque<PoolString> groupCommands = get_group_commands(name);
            // Push from the last command to ensure correct order
            typename Deque<PoolString>::Iterator it = groupCommands.end();
            --it;
            for ( ; it != groupCommands.begin(); --it) {
                commandQueue_.push_front(*it);
            }
            // Additional push for the first command (not handled by loop)
            commandQueue_.push_front(*it);
        }
    }

    /*!
        @brief  Executes the printing of a string.

        @param  command
                The print string command to execute.
    */
    void execute_print_string(Deque<Token> const & command) {
        Serial.println(command.front().get_value().c_str());
    }

    /*!
        @brief  Evaluates a postfix expression.

        @param  tokenQueue
                The postfix expression to be evaluated.

        @return The result stack after evaluation is complete.
                The stack may contain multiple values, depending on the input expression.
    */
    Deque<Token> evaluate_postfix(Deque<Token> const & tokenQueue) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokenStack;

        for (typename Deque<Token>::ConstIterator it = tokenQueue.begin(); it != tokenQueue.end(); ++it) {
            Token const & token = *it;
            if (token.is_unary_operator()) {
                Token operand = tokenStack.back();
                tokenStack.pop_back();
                Token result = evaluate_unary_operation(token, operand);
                tokenStack.push_back(result);
            }
            else if (token.is_operator()) {
                Token rhs = tokenStack.back();
                tokenStack.pop_back();
                Token lhs = tokenStack.back();
                tokenStack.pop_back();
                Token result = evaluate_operation(token, lhs, rhs);
                tokenStack.push_back(result);
            }
            else if (token.is_operand()) {
                // Instantly evaluate
                tokenStack.push_back(Token(TokenType::NUM_VAL, int_to_str(get_token_value(token))));
            }
            // Do nothing if function, shouldn't have more than one function 
            // within a command, and the function should have already been
            // extracted before calling evaluate_postfix
            //else if (token.is_function()) {
            //    tokenStack.push_back(token);
            //}
        }
        return tokenStack;
    }

    /*!
        @brief  Evaluates single unary operation.

        @param  operation
                The operation to be applied.
        
        @param  operand
                The operand to be operated on.

        @return The result of performing the operation.
                If any of the arguments are invalid, 
                a token containing "0" is returned.
    */
    Token evaluate_unary_operation(Token const & operation, Token const & operand) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int value = get_token_value(operand);
        Token result(TokenType::NUM_VAL);
        result.set_value(int_to_str(0));
        
        if (operation.is_unary_neg()) {
            result.set_value(int_to_str(-value));
        }
        else if (operation.is_logi_not()) {
            result.set_value(int_to_str(!value));
        }

        return result;
    }

    /*!
        @brief  Evaluates single binary operation.

        @param  operation
                The operation to be applied.
        
        @param  lhs
                The left hand side operand.
        
        @param  rhs
                The right hand side operand.

        @return The result of performing the operation.
                If any of the arguments are invalid, 
                a token containing "0" is returned.
    */
    Token evaluate_operation(Token const & operation, Token const & lhs, Token const & rhs) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int lhsValue = get_token_value(lhs);
        int rhsValue = get_token_value(rhs);
        Token result(TokenType::NUM_VAL);
        result.set_value(int_to_str(0));

        if (operation.is_equals()) {
            result.set_value(int_to_str(lhsValue == rhsValue));
        }
        else if (operation.is_l_equals()) {
            result.set_value(int_to_str(lhsValue <= rhsValue));
        }
        else if (operation.is_g_equals()) {
            result.set_value(int_to_str(lhsValue >= rhsValue));
        }
        else if (operation.is_less()) {
            result.set_value(int_to_str(lhsValue < rhsValue));
        }
        else if (operation.is_greater()) {
            result.set_value(int_to_str(lhsValue > rhsValue));
        }
        else if (operation.is_math_add()) {
            result.set_value(int_to_str(lhsValue + rhsValue));
        }
        else if (operation.is_math_sub()) {
            result.set_value(int_to_str(lhsValue - rhsValue));
        }
        else if (operation.is_math_mul()) {
            result.set_value(int_to_str(lhsValue * rhsValue));
        }
        else if (operation.is_math_div()) {
            result.set_value(int_to_str(lhsValue / rhsValue));
        }
        else if (operation.is_math_mod()) {
            result.set_value(int_to_str(lhsValue % rhsValue));
        }
        else if (operation.is_math_pow()) {
            result.set_value(int_to_str(power(lhsValue, rhsValue)));
        }
        else if (operation.is_logi_and()) {
            result.set_value(int_to_str(lhsValue && rhsValue));
        }
        else if (operation.is_logi_or()) {
            result.set_value(int_to_str(lhsValue || rhsValue));
        }
        return result;
    }

    /*!
        @brief  Evaluates a to the power of b.

        @param  a
                The base.
        
        @param  b
                The exponent.

        @return a raised to the power of b.
    */
    int power(int const & a, int const & b) {
        int result = 1;
        for (int i = 0; i < b; ++i) {
            result *= a;
        }
        return result;
    }

    /*!
        @brief  Returns the value of a token.

        @param  token
                The token to be evaluated.

        @return The value of the token.
                If the token is a number, that number is returned.
                If the token is a device, the status value of the device is returned.
                Otherwise, 0 is returned.
    */
    int get_token_value(Token const & token) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (token.is_num_val()) {
            return str_to_int(token.get_value());
        }
        else if (token.is_name()) {
            PoolString name(token.get_value());
            if (number_exists(name)) {
                return get_number_value(name);
            }
            else if (device_exists(name)) {
                return get_device_info(name, 2);
            }
        }
        return 0;
    }

    /*!
        @brief  Creates a number using the name and information given.

        @param  name
                The name of the number to be created
        
        @param  info
                The information about the number.
                The number value is expected to be the top token of the stack.
    */
    void create_number(PoolString const & name, Deque<Token> & info) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int value = str_to_int(info.back().get_value());
        machineState_.set_number(name, value);     
    }

    /*!
        @brief  Creates an LED using the name and information given.

        @param  name
                The name of the LED to be created
        
        @param  info
                The information about the LED.
                The LED brightness value is expected to be the top token of the stack,
                and the LED pin number is expected to be the second token from the top.
    */
    void create_led(PoolString const & name, Deque<Token> & info) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int brightness = str_to_int(info.back().get_value());
        info.pop_back();
        int pinNumber = str_to_int(info.back().get_value());
        pinMode(pinNumber, OUTPUT);
        analogWrite(pinNumber, (int)(brightness * 2.55));
        machineState_.set_device(name, DeviceType::LED, -1, pinNumber, brightness);
    }

    /*!
        @brief  Begins the creation of an if command group.

        @param  condition
                The result of evaluating the condition.
    */
    void create_if(int const & condition) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        enter_scope(InterpreterStatus::CREATING_IF);
        // Expand lastCondition_ if necessary
        while (lastCondition_.size() <= currScopeLevel_ + 1) {
            lastCondition_.push_back(-1);
        }
        lastCondition_[currScopeLevel_] = !!condition;
        ++currScopeLevel_;
    }

    /*!
        @brief  Adds a command to the if command group that is currently being created.

        @param  command
                The command to add to the if command group.
    */
    void add_to_if(PoolString const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Deque<Token> tokens = tokenizer_.tokenize(command);
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens.front().is_cl_paren()) {
            close_if();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(command);
            // Check if command introduces a '('
            if (tokens[tokens.size() - 2].is_op_paren()) {
                ++bracketParity_;
            }
            // Check if command is a single ')'
            else if (tokens.size() == 2 && tokens.front().is_cl_paren()) {
                --bracketParity_;
            }
        }
    }

    /*!
        @brief  Finishes the creation of an if command group,
                and adds its commands to the commandQueue if the condition is true.
    */
    void close_if() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int condition = lastCondition_[currScopeLevel_ - 1];
        if (condition) {
            // Special interpreter-only command to ensure we decrease scope level
            // after all the instructions in the if block are done
            commandQueue_.push_front(PoolString("DecreaseScopeLevel", *getPoolFunc_));
            // Add commands to commandQueue in reverse order,
            // since pushing from the front
            while (!commandBuffer_.is_empty()) {
                commandQueue_.push_front(commandBuffer_.back());
                commandBuffer_.pop_back();
            }
        }
        else {
            --currScopeLevel_;
        }
        exit_scope();
    }

    /*!
        @brief  Begins the creation of an else command group.
    */
    void create_else() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        enter_scope(InterpreterStatus::CREATING_ELSE);
        // Expand lastCondition_ if necessary
        while (lastCondition_.size() <= currScopeLevel_ + 1) {
            lastCondition_.push_back(-1);
        }
        ++currScopeLevel_;
    }

    /*!
        @brief  Adds a command to the else command group that is currently being created.

        @param  command
                The command to add to the else command group.
    */
    void add_to_else(PoolString const & command) {
        Log.verbose(F("%s\n"),  PRINT_FUNC);
        Deque<Token> tokens = tokenizer_.tokenize(command);
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens.front().is_cl_paren()) {
            close_else();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(command);
            // Check if command introduces a '('
            if (tokens[tokens.size() - 2].is_op_paren()) {
                ++bracketParity_;
            }
            // Check if command closes with a ')'
            else if (tokens.size() == 2 && tokens.front().is_cl_paren()) {
                --bracketParity_;
            }
        }
    }

    /*!
        @brief  Finishes the creation of an else command group,
                and adds its commands to the commandQueue if the else
                directly follows an if that had a condition which was false.
    */
    void close_else() {
        Log.verbose(F("%s\n"),  PRINT_FUNC);
        int condition = lastCondition_[currScopeLevel_ - 1];
        // No else should be run after this
        lastCondition_[currScopeLevel_ - 1] = -1;
        // Only run if last condition was false
        if (condition == 0) {
            // Special interpreter-only command to ensure we decrease scope level
            // after all the instructions in the if block are done
            commandQueue_.push_front(PoolString("DecreaseScopeLevel", *getPoolFunc_));
            // Add commands to commandQueue in reverse order,
            // since pushing from the front
            while (!commandBuffer_.is_empty()) {
                commandQueue_.push_front(commandBuffer_.back());
                commandBuffer_.pop_back();
            }
        }
        else {
            --currScopeLevel_;
        }
        exit_scope();
    }

    /*!
        @brief  Begins the creation of a command group.

        @param  name
                The name of the command group to be created.
    */
    void create_group(PoolString const & name) {
        Log.verbose(F("%s\n"),  PRINT_FUNC);
        enter_scope(InterpreterStatus::CREATING_GROUP);
        lastGroupName_ = name;
    }

    /*!
        @brief  Adds a command to the command group that is currently being created.

        @param  command
                The command to add to the command group.
    */
    void add_to_group(PoolString const & command) {
        Log.verbose(F("%s\n"),  PRINT_FUNC);
        Deque<Token> tokens = tokenizer_.tokenize(command);
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens.front().is_cl_paren()) {
            close_group();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(command);
            // Check if command introduces a '('
            if (tokens[tokens.size() - 2].is_op_paren()) {
                ++bracketParity_;
            }
            // Check if command closes with a ')'
            else if (tokens.size() == 2 && tokens.front().is_cl_paren()) {
                --bracketParity_;
            }
        }
    }

    /*!
        @brief  Finishes the creation of a command group.
    */
    void close_group() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        machineState_.set_group(lastGroupName_, commandBuffer_);
        lastGroupName_ = "";
        exit_scope();
    }

    /*!
        @brief  Sets the interpreter to enter a scope.

        @param  status
                The new status of the interpreter.
    */
    void enter_scope(InterpreterStatus status) {
        status_ = status;
    }

    /*!
        @brief  Exits the current scope of the interpreter.
    */
    void exit_scope() {
        status_ = InterpreterStatus::NORMAL;
        commandBuffer_.clear();
    }

private:
    GetAllocFunc * getAllocFunc_;
    GetPoolFunc * getPoolFunc_;

    Deque<PoolString> commandQueue_;
    Deque<PoolString> commandBuffer_;

    MachineState<> machineState_;
    PoolString     lastGroupName_;

    InterpreterStatus status_;

    /** Used to check for else/elseif blocks */
    int        currScopeLevel_ = 0;
    Deque<int> lastCondition_;

    int bracketParity_;

    Parser<>    parser_;
    Tokenizer<> tokenizer_;

};

} // namespace kty