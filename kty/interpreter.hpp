#pragma once

#include <kty/stl_impl.hpp>
#include <queue>
#include <stack>
#include <vector>
#if defined(ARDUINO)
#include <Servo.h>
#endif

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/string_utils.hpp>
#include <kty/tokenizer.hpp>
#include <kty/parser.hpp>

namespace kty {

/** The various device types possible */
enum DeviceType {
    NUM,
    LED, SERVO,
    UNKNOWN_DEVICE,
};

/*!
    @brief  Converts a device type to its string equivalent

    @param  deviceType
            The device type to be converted
    
    @return The string version of the given device type.
            If the device type given is invalid, "UNKNOWN_DEVICE" is returned.
*/
std::string device_type_to_str(DeviceType deviceType) {
    switch (deviceType) {
    case NUM:
        return "NUM";
    case LED:
        return "LED";
    case SERVO:
        return "SERVO";
    default:
        return "UNKNOWN_DEVICE";
    };
}

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
template <class Alloc, class StringPool>
class Interpreter {

public:
    /** The type of pool string used in the interpreter */
    typedef PoolString<StringPool> poolstring_t;
    /*!
        @brief  Default interpreter constructor.

        @param  alloc
                Allocator for the class to use.
        
        @param  stringPool
                String pool for the class to use.
    */
    Interpreter(Alloc& alloc, StringPool& stringPool)
            : alloc_(alloc), stringPool_(stringPool),
              deviceNames_(alloc), deviceTypes_(alloc), deviceInfo_0_(alloc), deviceInfo_1_(alloc), deviceInfo_2_(alloc),
              lastGroupName_(stringPool), groupNames_(alloc), groupCommands_(stringPool, alloc),
              toPopLastIfCondition_(alloc), lastIfCondition_(alloc), 
              commandQueue_(alloc), commandBuffer_(alloc) {
        status_ = InterpreterStatus::NORMAL;
        lastIfCondition_.push_back(-1); // Last if condition is null
    }

    /*!
        @brief  Gets the prefix(if any) for the user prompt.

        @return The prefix for the user prompt.
                If there is no required prefix, an empty string is returned.
    */
    poolstring_t get_prompt_prefix() {
        poolstring_t prefix(stringPool_);
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
        @brief  Checks whether a device exists.

        @param  name
                The name of the device to search for.

        @return The index of the device in the deques.
                If the device does not exist, -1 is returned.
    */
    int device_exists(poolstring_t const & name) {
        for (int i = 0; i < deviceNames_.size(); ++i) {
            if (deviceNames_[i] == name) {
                Log.trace(F("Interpreter::device_exists found %s at index %d\n"), name.c_str(), i);
                return i;
            }
        }
        Log.trace(F("Interpreter::device_exists unable to find %s\n"), name.c_str());
        return -1;
    }

    /*!
        @brief  Checks whether a group exists.

        @param  name
                The name of the group to search for.

        @return The index of the group in the deques.
                If the group does not exist, -1 is returned.
    */
    int group_exists(poolstring_t const & name) {
        for (int i = 0; i < groupNames_.size(); ++i) {
            if (groupNames_[i] == name) {
                Log.trace(F("Interpreter::group_exists found %s at index %d\n"), name.c_str(), i);
                return i;
            }
        }
        Log.trace(F("Interpreter::group_exists unable to find %s\n"), name.c_str());
        return -1;
    }

    /*!
        @brief  Gets the type of device.

        @param  name
                The device name.
        
        @return The type of the device.
                Returns an unknown device if it does not exist.
    */
    DeviceType get_device_type(poolstring_t const & name) {
        int deviceIdx = device_exists(name);
        if (deviceIdx == -1) {
            return DeviceType::UNKNOWN_DEVICE;
        }
        return deviceTypes_[deviceIdx];
    }

    /*!
        @brief  Gets the info of device.

        @param  name
                The device name.

        @param  i
                The info index.
        
        @return The info of the device.
                Returns -1 if it does not exist.
    */
    int get_device_info(poolstring_t const & name, int const & i) {
        int deviceIdx = device_exists(name);
        if (deviceIdx == -1) {
            return -1;
        }
        switch (i) {
        case 0:
            return deviceInfo_0_[deviceIdx];
        case 1:
            return deviceInfo_1_[deviceIdx];
        case 2:
            return deviceInfo_2_[deviceIdx];
        };
        return -1;
    }

    /*!
        @brief  Executes a given command. 
                This method takes the current interpreter status into account.

        @param  command
                The raw string command to execute.
    */
    void execute(poolstring_t command) {
        Log.trace(F("Interpreter::execute executing %s\n"), command.c_str());
        remove_str_whitespace(command);
        // Nothing to execute
        if (command.strlen() == 0) {
            Log.trace(F("Interpreter::execute no command to execute, done\n"));
            return;
        }
        std::vector<Token> tokens;
        switch (status_) {
        case NORMAL:
            tokens = tokenizer_.tokenize(std::string(command.c_str()));
            tokens = parser_.parse(tokens);
            execute(tokens);
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
        execute_command_queue();
        cleanup_after_command_queue_execute();
        Log.trace(F("Interpreter::execute done executing %s\n"), command.c_str());
    }

    /*!
        @brief  Executes all the commands still in the command queue, if any.
    */
    void execute_command_queue() {
        // No commands to execute
        if (commandQueue_.size() == 0) {
            return;
        }
        while (!commandQueue_.is_empty()) {
            poolstring_t command(commandQueue_.front());
            commandQueue_.pop_front();
            execute(command);
        }
    }

    /*!
        @brief  Executes all the cleanup for internal state after executing 
                the commands in the command queue.
    */
    void cleanup_after_command_queue_execute() {
        //Serial.print("cleanup size ");
        //Serial.print(toPopLastIfCondition_.size());
        //Serial.print(" ");
        //Serial.println(lastIfCondition_.size());
        if (!toPopLastIfCondition_.is_empty()) {
            if (toPopLastIfCondition_.back()) {
                lastIfCondition_.pop_back();
            }
            toPopLastIfCondition_.pop_back();
        }
    }

    /*!
        @brief  Executes a given command.

        @param  command
                The command to execute.
                Tokens in command are assumed to be in postfix notation.
    */
    void execute(std::vector<Token> const & command) {
        // Nothing to execute
        if (command.empty()) {
            return;
        }
        if (command.back().is_if()) {
            execute_if(command);
        }
        else if (command.back().is_else()) {
            execute_else(command);
        }
        else {
            // Everything other than if and else, last if condition is no longer valid
            lastIfCondition_.back() = -1;
            if (command[0].is_name()) {
                // Printing information
                if (command.size() == 1) {
                    execute_print_info(command);
                }
                // Creation of a device/group
                else if (command.back().is_create_num() || 
                         command.back().is_create_led() || 
                         command.back().is_create_servo() || 
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
            else if (command[0].is_string()) {
                lastIfCondition_.back() = -1;
                execute_print_string(command);
            }
        }
    }

    /*!
        @brief  Executes the print information command.

        @param  command
                The command to execute.
    */
    void execute_print_info(std::vector<Token> const & command) {
        poolstring_t name(stringPool_, command[0].value.c_str());
        int deviceIdx = device_exists(name);
        int groupIdx = group_exists(name);
        if (deviceIdx >= 0) {
            // Print device info
        }
        else if (groupIdx >= 0) {
            // Print out group commands
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
    void execute_if(std::vector<Token> const & command) {
        std::queue<Token> tokenQueue;
        // Skip the if token at the end
        for (int i = 0; i < command.size() - 1; ++i) {
            tokenQueue.push(command[i]);
        }
        std::stack<Token> result = evaluate_postfix(tokenQueue);
        create_if(result.top().value);
    }

    /*!
        @brief  Executes the else command.

        @param  command
                The command to execute.
    */
    void execute_else(std::vector<Token> const & command) {
        // No condition to extract for else, immediately create group
        create_else();
    }

    /*!
        @brief  Executes the different creation commands.

        @param  command
                The command to execute.
    */
    void execute_create(std::vector<Token> const & command) {
        std::queue<Token> tokenQueue;
        // Skip the create token at the end
        for (int i = 0; i < command.size() - 1; ++i) {
            tokenQueue.push(command[i]);
        }
        poolstring_t name(stringPool_, tokenQueue.front().value.c_str());
        Token createToken = command[command.size() - 1];
        tokenQueue.pop();

        std::stack<Token> result = evaluate_postfix(tokenQueue);

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
    void execute_move_by(std::vector<Token> const & command) {
        std::queue<Token> tokenQueue;
        // Skip the move by or move by for token at the end
        for (int i = 0; i < command.size() - 1; ++i) {
            tokenQueue.push(command[i]);
        }
        poolstring_t name(stringPool_, tokenQueue.front().value.c_str());
        // Nothing to move
        int deviceIdx = device_exists(name);
        if (deviceIdx == -1) {
            Serial.print(F("Error: "));
            Serial.print(name.c_str());
            Serial.println(F(" does not exist"));
            return;
        }
        // Cannot be moved
        DeviceType deviceType = deviceTypes_[deviceIdx];
        if (deviceType != DeviceType::NUM &&
            deviceType != DeviceType::LED &&
            deviceType != DeviceType::SERVO) {
            Serial.print(F("Error: move command is not valid for"));
            Serial.println(name.c_str());
            return;
        }
        Token createToken = command[command.size() - 1];
        tokenQueue.pop();

        std::stack<Token> result = evaluate_postfix(tokenQueue);

        int displacement, durationMs = 0;
        // There will be an additional argument if it is move by for
        if (result.size() > 1) {
            durationMs = str_to_int(result.top().value);
            result.pop();
        }
        displacement = str_to_int(result.top().value);

        // Execute move
        int & deviceInfo1 = deviceInfo_1_[deviceIdx];
        int & deviceInfo2 = deviceInfo_2_[deviceIdx];
        deviceInfo2 += displacement;
        switch (deviceType) {
        case NUM:
            break;
        case LED:
            analogWrite(deviceInfo1, deviceInfo2 * 2.55);
            break;
        };
        if (command.back().is_move_by_for()) {
            delay(durationMs);
            deviceInfo2 -= displacement;
            switch (deviceType) {
            case NUM:
                break;
            case LED:
                analogWrite(deviceInfo1, deviceInfo2 * 2.55);
                break;
            };
        }
    }

    /*!
        @brief  Executes the running of a command group.
                Essentially adds the commands in the command group to the
                command queue to be executed later.

        @param  command
                The command to execute.
    */
    void execute_run_group(std::vector<Token> const & command) {
        std::queue<Token> tokenQueue;
        for (int i = 0; i < command.size() - 1; ++i) {
            tokenQueue.push(command[i]);
        }
        // Extract name of group and check if it exists
        poolstring_t name(stringPool_, tokenQueue.front().value.c_str());
        int groupIdx = group_exists(name);
        if (groupIdx == -1) {
            Log.warning(F("Interpreter::execute_run_group %s does not exist\n"), name.c_str());
            return;
        }
        // Extract number of times to run group
        std::stack<Token> result = evaluate_postfix(tokenQueue);
        int numTimes = str_to_int(result.top().value);
        Log.trace(F("Running group = %s, %d times\n"), name.c_str(), numTimes);
        // Push command for one more call to run the group
        if (numTimes > 1) {
            poolstring_t repeatGroupCommand(stringPool_);
            repeatGroupCommand += name.c_str();
            repeatGroupCommand += "RunGroup(";
            repeatGroupCommand += int_to_str(numTimes - 1, stringPool_);
            repeatGroupCommand += ")";
            commandQueue_.push_front(repeatGroupCommand);
            Log.trace(F("Interpreter::execute_run_group pushing %s to the front of the command queue for next round\n"), repeatGroupCommand.c_str());
        }
        // Push commands from group to command queue
        int numGroupCommands = groupCommands_.size(groupIdx);
        Log.trace(F("Interpreter::execute_run_group %d commands to add to command queue\n"), numGroupCommands);
        for (int i = numGroupCommands - 1; i >= 0; --i) {
            poolstring_t commandString = groupCommands_.get_str(groupIdx, i);
            Log.trace(F("Interpreter::execute_run_group pushing %s to front of command queue\n"), commandString.c_str());
            commandQueue_.push_front(commandString);
        }
    }

    /*!
        @brief  Executes the printing of a string.

        @param  command
                The print string command to execute.
    */
    void execute_print_string(std::vector<Token> const & command) {
        Serial.println(command[0].value.c_str());
    }

    /*!
        @brief  Evaluates a postfix expression.

        @param  tokenQueue
                The postfix expression to be evaluated.

        @return The result stack after evaluation is complete.
                The stack may contain multiple values, depending on the input expression.
    */
    std::stack<Token> evaluate_postfix(std::queue<Token> & tokenQueue) {
        std::stack<Token> tokenStack;

        while (!tokenQueue.empty()) {
            Token token = tokenQueue.front();
            tokenQueue.pop();
            if (token.is_unary_operator()) {
                Token operand = tokenStack.top();
                tokenStack.pop();
                Token result = evaluate_unary_operation(token, operand);
                tokenStack.push(result);
            }
            else if (token.is_operator()) {
                Token rhs = tokenStack.top();
                tokenStack.pop();
                Token lhs = tokenStack.top();
                tokenStack.pop();
                Token result = evaluate_operation(token, lhs, rhs);
                tokenStack.push(result);
            }
            else if (token.is_operand()) {
                tokenStack.push(token);
            }
            else if (token.is_function()) {
                tokenStack.push(token);
            }
        }
        return tokenStack;
    }

    /*!
        @brief  Evaluates single unary operation.

        @param  op
                The operation to be applied.
        
        @param  operand
                The operand to be operated on.

        @return The result of performing the operation.
                If any of the arguments are invalid, 
                a token containing "0" is returned.
    */
    Token evaluate_unary_operation(Token const & op, Token const & operand) {
        int value = get_token_value(operand);
        Token result(TokenType::NUM_VAL);
        result.value = "0";
        
        if (op.type == TokenType::UNARY_NEG) {
            result.value = int_to_str(-value);
        }
        else if (op.type == TokenType::LOGI_NOT) {
            result.value = int_to_str(!value);
        }

        return result;
    }

    /*!
        @brief  Evaluates single binary operation.

        @param  op
                The operation to be applied.
        
        @param  lhs
                The left hand side operand.
        
        @param  rhs
                The right hand side operand.

        @return The result of performing the operation.
                If any of the arguments are invalid, 
                a token containing "0" is returned.
    */
    Token evaluate_operation(Token const & op, Token const & lhs, Token const & rhs) {
        int lhsValue = get_token_value(lhs);
        int rhsValue = get_token_value(rhs);
        Token result(TokenType::NUM_VAL);
        result.value = "0";

        if (op.type == TokenType::EQUALS) {
            result.value = int_to_str(lhsValue == rhsValue);
        }
        else if (op.type == TokenType::L_EQUALS) {
            result.value = int_to_str(lhsValue <= rhsValue);
        }
        else if (op.type == TokenType::G_EQUALS) {
            result.value = int_to_str(lhsValue >= rhsValue);
        }
        else if (op.type == TokenType::LESS) {
            result.value = int_to_str(lhsValue < rhsValue);
        }
        else if (op.type == TokenType::GREATER) {
            result.value = int_to_str(lhsValue > rhsValue);
        }
        else if (op.type == TokenType::MATH_ADD) {
            result.value = int_to_str(lhsValue + rhsValue);
        }
        else if (op.type == TokenType::MATH_SUB) {
            result.value = int_to_str(lhsValue - rhsValue);
        }
        else if (op.type == TokenType::MATH_MUL) {
            result.value = int_to_str(lhsValue * rhsValue);
        }
        else if (op.type == TokenType::MATH_DIV) {
            result.value = int_to_str(lhsValue / rhsValue);
        }
        else if (op.type == TokenType::MATH_MOD) {
            result.value = int_to_str(lhsValue % rhsValue);
        }
        else if (op.type == TokenType::MATH_POW) {
            result.value = int_to_str(power(lhsValue, rhsValue));
        }
        else if (op.type == TokenType::LOGI_AND) {
            result.value = int_to_str(lhsValue && rhsValue);
        }
        else if (op.type == TokenType::LOGI_OR) {
            result.value = int_to_str(lhsValue || rhsValue);
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
        int result = a;
        for (int i = 1; i < b; ++i) {
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
        if (token.is_number()) {
            return str_to_int(token.value);
        }
        else if (token.is_name()) {
            poolstring_t name(stringPool_, token.value.c_str());
            int deviceIdx = device_exists(name);
            if (deviceIdx >= 0) {
                return deviceInfo_2_[deviceIdx];
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
    void create_number(poolstring_t const & name, std::stack<Token> & info) {
        int deviceIdx = device_exists(name);
        int value = str_to_int(info.top().value);
        Log.trace(F("Interpreter::create_number %s = %d\n"), name.c_str(), value);
        // Create new device
        if (deviceIdx == -1) {
            Log.trace(F("Interpreter::create_number creating new device\n"));
            deviceNames_.push_front(name);
            deviceTypes_.push_front(DeviceType::NUM);
            deviceInfo_0_.push_front(0);
            deviceInfo_1_.push_front(0);
            deviceInfo_2_.push_front(value);
        }
        // Modify existing device
        else {
            Log.trace(F("Interpreter::create_number modifying existing device, index %d\n"), deviceIdx);
            deviceNames_[deviceIdx] = name;
            deviceTypes_[deviceIdx] = DeviceType::NUM;
            deviceInfo_0_[deviceIdx] = 0;
            deviceInfo_1_[deviceIdx] = 0;
            deviceInfo_2_[deviceIdx] = value;
        }
        Log.trace(F("Interpreter::create_number created\n"));        
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
    void create_led(PoolString<StringPool> const & name, std::stack<Token> & info) {
        int deviceIdx = device_exists(name);
        int brightness = str_to_int(info.top().value);
        info.pop();
        int pinNumber = str_to_int(info.top().value);
        pinMode(pinNumber, OUTPUT);
        analogWrite(pinNumber, (int)(brightness * 2.55));
        Log.trace(F("Interpreter::create_led %s = pin %d brightness %d\n"), name.c_str(), pinNumber, brightness);
        if (deviceIdx == -1) {
            deviceNames_.push_front(name);
            deviceTypes_.push_front(DeviceType::LED);
            deviceInfo_0_.push_front(0);
            deviceInfo_1_.push_front(pinNumber);
            deviceInfo_2_.push_front(brightness);
        }
        else {
            deviceNames_[deviceIdx] = name;
            deviceTypes_[deviceIdx] = DeviceType::LED;
            deviceInfo_0_[deviceIdx] = 0;
            deviceInfo_1_[deviceIdx] = pinNumber;
            deviceInfo_2_[deviceIdx] = brightness;
        }
    }

    /*!
        @brief  Begins the creation of an if command group.

        @param  condition
                The result of evaluating the condition.
    */
    void create_if(std::string const & condition) {
        increase_nested_level(InterpreterStatus::CREATING_IF);
        poolstring_t cond(stringPool_, condition.c_str());
        commandBuffer_.push_back(cond);       
        Log.trace(F("Interpreter::create_if condition = %s\n"), cond.c_str());
    }

    /*!
        @brief  Adds a command to the if command group that is currently being created.

        @param  command
                The command to add to the if command group.
    */
    void add_to_if(poolstring_t const & command) {
        std::vector<Token> tokens = tokenizer_.tokenize(std::string(command.c_str()));
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens[0].is_right_bracket()) {
            close_if();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(command);
            // Check if command introduces a '('
            if (tokens[tokens.size() - 2].is_left_bracket()) {
                ++bracketParity_;
            }
            // Check if command closes with a ')'
            else if (tokens.size() == 2 && tokens[0].is_right_bracket()) {
                --bracketParity_;
            }
        }
    }

    /*!
        @brief  Finishes the creation of an if command group,
                and adds its commands to the commandQueue if the condition is true.
    */
    void close_if() {
        bool evaluatedCondition = *(commandBuffer_[0].c_str()) == '1';
        lastIfCondition_.back() = evaluatedCondition;
        if (evaluatedCondition) {
            Log.trace(F("Interpreter::close_if condition is true\n"));
            // Add commands to commandQueue in reverse order,
            // since pushing from the front
            for (int i = commandBuffer_.size() - 1; i > 0; --i) {
                commandQueue_.push_front(commandBuffer_[i]);
            }
            // Push for any commands that might require in nested scope
            lastIfCondition_.push_back(-1);
            toPopLastIfCondition_.push_back(true);
        }
        decrease_nested_level();
        Log.trace(F("Interpreter::close_if closing\n"));
    }

    /*!
        @brief  Begins the creation of an else command group.
    */
    void create_else() {
        increase_nested_level(InterpreterStatus::CREATING_ELSE);
        Log.trace(F("Interpreter::create_else\n"));
        // Only execute else block if last if condition was false
        poolstring_t ourCondition(stringPool_);
        if (lastIfCondition_.back() == 0) {
            // We should execute this else
            Log.trace(F("Interpreter::create_else should execute\n"));
            ourCondition = "1";
            commandBuffer_.push_back(ourCondition);
        }
        else {
            // We should not execute this else
            Log.trace(F("Interpreter::create_else should not execute\n"));
            ourCondition = "0";
            commandBuffer_.push_back(ourCondition);
        }
    }

    /*!
        @brief  Adds a command to the else command group that is currently being created.

        @param  command
                The command to add to the else command group.
    */
    void add_to_else(poolstring_t const & command) {
        std::vector<Token> tokens = tokenizer_.tokenize(std::string(command.c_str()));
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens[0].is_right_bracket()) {
            close_else();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(command);
            //commandBuffer_.push_back(stringPool_.allocate_idx());
            //stringPool_.strcpy(commandBuffer_.back(), command.c_str());
            // Check if command introduces a '('
            if (tokens[tokens.size() - 2].is_left_bracket()) {
                ++bracketParity_;
            }
            // Check if command closes with a ')'
            else if (tokens.size() == 2 && tokens[0].is_right_bracket()) {
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
        bool evaluatedCondition = *(commandBuffer_[0].c_str()) == '1';
        // Definitely no else after this
        lastIfCondition_.back() = -1;
        if (evaluatedCondition) {
            Log.trace(F("Interpreter::close_else will be executed\n"));
            // Add commands to commandQueue in reverse order,
            // since pushing from the front
            for (int i = commandBuffer_.size() - 1; i >= 0; --i) {
                commandQueue_.push_front(commandBuffer_[i]);
            }
            // Push for any commands that might require in nested scope
            lastIfCondition_.push_back(-1);
            toPopLastIfCondition_.push_back(true);
        }
        decrease_nested_level();
        Log.trace(F("Interpreter::close_else closing\n"));
    }

    /*!
        @brief  Begins the creation of a command group.

        @param  name
                The name of the command group to be created.
    */
    void create_group(poolstring_t const & name) {
        increase_nested_level(InterpreterStatus::CREATING_GROUP);
        int groupIdx = group_exists(name);
        if (groupIdx == -1) {
            Log.trace(F("Interpreter::create_group new group %s\n"), name.c_str());
            groupNames_.push_front(name);
            groupCommands_.push_front();
        }
        else {
            Log.trace(F("Interpreter::create_group modifying existing group %s\n"), name.c_str());
            groupNames_[groupIdx] = name;
            groupCommands_.clear(groupIdx);
        }
        lastGroupName_ = name;
    }

    /*!
        @brief  Adds a command to the command group that is currently being created.

        @param  command
                The command to add to the command group.
    */
    void add_to_group(poolstring_t const & command) {
        Log.trace(F("Interpreter::add_to_group(%s)\n"), command.c_str());
        std::vector<Token> tokens = tokenizer_.tokenize(std::string(command.c_str()));
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens[0].is_right_bracket()) {
            Log.trace(F("Interpreter::add_to_group group will be closed\n"));
            close_group();
        }
        // Add command to group
        else {
            Log.trace(F("Interpreter::add_to_group adding %s to group %s\n"), command.c_str(), lastGroupName_.c_str());
            int groupIdx = group_exists(lastGroupName_);
            groupCommands_.push_back(groupIdx, command);
            // Check if command introduces a '('
            if (tokens[tokens.size() - 2].is_left_bracket()) {
                ++bracketParity_;
            }
            // Check if command closes with a ')'
            else if (tokens.size() == 2 && tokens[0].is_right_bracket()) {
                --bracketParity_;
            }
            Log.trace(F("Interpreter::add_to_group bracket parity %d\n"), bracketParity_);
        }
    }

    /*!
        @brief  Finishes the creation of a command group.
    */
    void close_group() {
        Log.trace(F("Interpreter::close_group closing %s\n"), lastGroupName_.c_str());
        lastGroupName_ = "";
        decrease_nested_level();
    }

    /*!
        @brief  Increases the nested level of the interpreter.

        @param  status
                The new status of the interpreter.
    */
    void increase_nested_level(InterpreterStatus status) {
        status_ = status;
    }

    /*!
        @brief  Decreases the nested level of the interpreter
    */
    void decrease_nested_level() {
        status_ = InterpreterStatus::NORMAL;
        commandBuffer_.clear();
    }

private:
    Alloc& alloc_;
    StringPool& stringPool_;

    Deque<poolstring_t, Alloc> commandQueue_;
    Deque<poolstring_t, Alloc> commandBuffer_;

    Deque<poolstring_t, Alloc> deviceNames_;
    Deque<DeviceType, Alloc>   deviceTypes_;
    Deque<int, Alloc>          deviceInfo_0_;
    Deque<int, Alloc>          deviceInfo_1_;
    Deque<int, Alloc>          deviceInfo_2_;

    poolstring_t                             lastGroupName_;
    Deque<poolstring_t, Alloc>               groupNames_;
    DequeDequePoolString<StringPool, Alloc>  groupCommands_;

    InterpreterStatus status_;

    /** Used to check for else/elseif blocks */
    Deque<int, Alloc> lastIfCondition_;
    Deque<int, Alloc> toPopLastIfCondition_;
    int bracketParity_;

    Parser parser_;
    Tokenizer tokenizer_;

};

} // namespace kty