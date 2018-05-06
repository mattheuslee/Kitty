#pragma once

#include <kty/stl_impl.hpp>
#include <map>
#include <queue>
#include <stack>
#include <vector>
#if defined(ARDUINO)
#include <Servo.h>
#endif

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/string_utils.hpp>
#include <kty/tokenizer.hpp>
#include <kty/parser.hpp>

namespace kty {

/** The various device types possible */
enum DeviceType {
    NUM,
    LED, SERVO, GROUP,
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
    case GROUP:
        return "GROUP";
    default:
        return "UNKNOWN_DEVICE";
    };
}

/*!
    @brief  Class that holds information about a device
*/
struct Device {

    /** The type of the device */    
    DeviceType type;
    /** The name of the device */    
    std::string name;
    /** Information about the device. The format of this member varies depending on the type of the device. */    
    std::vector<std::string> info;

    #if defined(ARDUINO)
    /** A servo object to be used if the device is a servo. */    
    Servo servo;
    #endif

    /*!
        @brief  Constructor for device

        @param  _type
                The type of the device.
    */
    explicit Device(DeviceType _type = DeviceType::UNKNOWN_DEVICE) : type(_type) {
    }

    /*!
        @brief  Gets a debugging string representation of the device.

        @return The debugging string representation of the device.
    */
    std::string debug_str() const {
        std::string str = "Device(" + device_type_to_str(type) + ")";
        return str;
    }

    /*!
        @brief  Gets an informational string representation of the device.
                This string is suitable for display to the user.

        @return The informational string representation of the device.
    */
    std::string info_str() const {
        std::string str = name + ": ";
        switch (type) {
        case NUM:
            str += "Number storing " + info[0];
            break;
        case LED:
            str += "LED using pin " + info[0] + " ("+ info[1] + "%)";
            break;
        case SERVO:
            str += "Servo using pin " + info[0] + " ("+ info[1] + " degrees)";
            break;
        case GROUP:
            str += "Command Group containing the command(s):";
            for (int i = 0; i < info.size(); ++i) {
                str += "\n    " + info[i];
            }
            break;
        case UNKNOWN_DEVICE:
        default:
            return "UNKNOWN_DEVICE";            
        };
        return str;
    }

    /*!
        @brief  Sets the device to a given number value.

        @param  value
                The value to set the device to.
    */
    void set_to(int const & value) {
        switch (type) {
        case NUM:
            info[0] = int_to_str(value);
            break;
        case LED:
            info[1] = int_to_str(value);
            analogWrite(str_to_int(info[0]), (int)(value * 2.55));
            break;
        };
    }

    /*!
        @brief  Checks if this device is a number.

        @return True if this device is a number, false otherwise.
    */
    bool is_number() {
        return type == DeviceType::NUM;
    }

    /*!
        @brief  Checks if this device is an LED.

        @return True if this device is an LED, false otherwise.
    */
    bool is_led() {
        return type == DeviceType::LED;
    }

    /*!
        @brief  Checks if this device is a servo.

        @return True if this device is a servo, false otherwise.
    */
    bool is_servo() {
        return type == DeviceType::SERVO;
    }

    /*!
        @brief  Checks if this device is a group.

        @return True if this device is a group, false otherwise.
    */
    bool is_group() {
        return type == DeviceType::GROUP;
    }
};

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
    /*!
        @brief  Default interpreter constructor.

        @param  alloc
                Allocator for the class to use.
        
        @param  stringPool
                String pool for the class to use.
    */
    Interpreter(Alloc& alloc, StringPool& stringPool)
            : toPopLastIfCondition_(alloc), lastIfCondition_(alloc), commandQueue_(alloc), commandBuffer_(alloc), stringPool_(stringPool) {
        status_ = InterpreterStatus::NORMAL;
        lastIfCondition_.push_back(-1); // Last if condition is null
    }

    /*!
        @brief  Gets the prefix(if any) for the user prompt.

        @return The prefix for the user prompt.
                If there is no required prefix, an empty string is returned.
    */
    std::string get_prompt_prefix() {
        switch (status_) {
        case NORMAL:
            return "";
        case CREATING_GROUP:
            return "(" + std::string(commandBuffer_[0].c_str()) + ") ";
        case CREATING_IF:
            return "(IF) ";
        case CREATING_ELSE:
            return "(ELSE) ";
        default:
            return "";
        }
    }

    /*!
        @brief  Executes a given command. 
                This method takes the current interpreter status into account.

        @param  command
                The raw string command to execute.
    */
    void execute(std::string command) {
        // Nothing to execute
        if (command.size() == 0) {
            return;
        }
        std::vector<Token> tokens;
        switch (status_) {
        case NORMAL:
            tokens = tokenizer_.tokenize(command);
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
            std::string command(commandQueue_.front().c_str());
            //std::string command(stringPool_.c_str(commandQueue_.front()));
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
        std::string name = command[0].value;
        if (devices_.find(name) != devices_.end()) {
            Serial.println(devices_.find(name)->second.info_str().c_str());
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
        std::string name = tokenQueue.front().value;
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
        std::string name = tokenQueue.front().value;
        // Nothing to move
        if (devices_.find(name) == devices_.end()) {
            Serial.print(F("Error: "));
            Serial.print(name.c_str());
            Serial.println(F(" does not exist"));
            return;
        }
        // Cannot be moved
        if (!devices_.find(name)->second.is_number() &&
            !devices_.find(name)->second.is_led() &&
            !devices_.find(name)->second.is_servo()) {
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

        int originalValue = str_to_int(devices_.find(name)->second.info.back());
        devices_.find(name)->second.set_to(originalValue + displacement);
        if (command.back().is_move_by_for()) {
            delay(durationMs);
            devices_.find(name)->second.set_to(originalValue);
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
        std::string name = tokenQueue.front().value;
        std::stack<Token> result = evaluate_postfix(tokenQueue);
        int numTimes = str_to_int(result.top().value);
        std::vector<std::string> commands = get_device(name).info;
        Log.trace(F("Running group = %s\n"), name.c_str());
        // Add command for one more call to run the group
        if (numTimes > 1) {
            commandQueue_.push_front(PoolString<StringPool>(stringPool_));
            commandQueue_.front() = name.c_str();
            commandQueue_.front() += "RunGroup(";
            commandQueue_.front() += int_to_str(numTimes - 1, stringPool_);
            commandQueue_.front() += ")";
            // Replaces
            //commandQueue_.push_front(name + "RunGroup(" + int_to_str(numTimes - 1) + ")");
        }
        for (int j = 0; j < commands.size(); ++j) {
            // Push in reverse order since pushing from the front
            commandQueue_.push_front(PoolString<StringPool>(stringPool_, commands[commands.size() - j - 1].c_str()));
            //commandQueue_.push_front(stringPool_.allocate_idx());
            //stringPool_.strcpy(commandQueue_.front(), commands[commands.size() - j - 1].c_str());
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
        @brief  Retrieves a device by name.

        @param  name
                The name of the device to retrieve.

        @return The found device.
                If no device is found with the given name, 
                a placeholder unknown device is returned.
    */
    Device get_device(std::string const & name) {
        if (devices_.find(name) == devices_.end()) {
            return Device(DeviceType::UNKNOWN_DEVICE);
        }
        return devices_[name];
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
            Device device = devices_[token.value];
            if (device.is_number()) {
                return str_to_int(device.info[0]);
            }
            else if (device.is_led()) {
                return str_to_int(device.info[1]);
            }
            else if (device.is_servo()) {
                return str_to_int(device.info[2]);
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
    void create_number(std::string const & name, std::stack<Token> & info) {
        Device number(DeviceType::NUM);
        number.name = name;
        number.info.push_back(info.top().value);
        devices_[name] = number;
        Log.trace(F("Creating number %s\n"), info.top().value.c_str());
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
    void create_led(std::string const & name, std::stack<Token> & info) {
        Device number(DeviceType::LED);
        number.name = name;
        int brightness = str_to_int(info.top().value);
        info.pop();
        int pinNumber = str_to_int(info.top().value);
        pinMode(pinNumber, OUTPUT);
        analogWrite(pinNumber, (int)(brightness * 2.55));
        number.info.push_back(int_to_str(pinNumber));
        number.info.push_back(int_to_str(brightness));
        devices_[name] = number;
        Log.trace(F("Creating LED, pin %d brightness %d\n"), pinNumber, brightness);
    }

    /*!
        @brief  Begins the creation of an if command group.

        @param  condition
                The result of evaluating the condition.
    */
    void create_if(std::string const & condition) {
        increase_nested_level(InterpreterStatus::CREATING_IF);
        commandBuffer_.push_back(PoolString<StringPool>(stringPool_, condition.c_str()));       
        //commandBuffer_.push_back(stringPool_.allocate_idx());
        //stringPool_.strcpy(commandBuffer_.back(), condition.c_str());
        Log.trace(F("Creating if\n"));
    }

    /*!
        @brief  Adds a command to the if command group that is currently being created.

        @param  command
                The command to add to the if command group.
    */
    void add_to_if(std::string const & command) {
        std::vector<Token> tokens = tokenizer_.tokenize(command);
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens[0].is_right_bracket()) {
            close_if();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(PoolString<StringPool>(stringPool_, command.c_str()));            
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
        @brief  Finishes the creation of an if command group,
                and adds its commands to the commandQueue if the condition is true.
    */
    void close_if() {
        bool evaluatedCondition = *(commandBuffer_[0].c_str()) == '1';
        lastIfCondition_.back() = evaluatedCondition;
        if (evaluatedCondition) {
            Log.trace(F("If condition is true\n"));
            // Add commands to commandQueue in reverse order,
            // since pushing from the front
            for (int i = commandBuffer_.size() - 1; i > 0; --i) {
                commandQueue_.push_front(commandBuffer_[i]);
                //stringPool_.inc_ref_count(commandBuffer_[i]);
                // Replaces
                //commandQueue_.push_front(std::string(stringPool_.c_str(commandBuffer_[i])));
            }
            // Push for any commands that might require in nested scope
            lastIfCondition_.push_back(-1);
            toPopLastIfCondition_.push_back(true);
        }
        decrease_nested_level();
        Log.trace(F("Closing if\n"));
    }

    /*!
        @brief  Begins the creation of an else command group.
    */
    void create_else() {
        increase_nested_level(InterpreterStatus::CREATING_ELSE);
        Log.trace(F("Creating else\n"));
        // Only execute else block if last if condition was false
        if (lastIfCondition_.back() == 0) {
            commandBuffer_.push_back(PoolString<StringPool>(stringPool_, "1"));
            //commandBuffer_.push_back(stringPool_.allocate_idx());
            //stringPool_.strcpy(commandBuffer_.back(), "1");
        }
        else {
            commandBuffer_.push_back(PoolString<StringPool>(stringPool_, "0"));
            //commandBuffer_.push_back(stringPool_.allocate_idx());
            //stringPool_.strcpy(commandBuffer_.back(), "0");
        }
    }

    /*!
        @brief  Adds a command to the else command group that is currently being created.

        @param  command
                The command to add to the else command group.
    */
    void add_to_else(std::string const & command) {
        std::vector<Token> tokens = tokenizer_.tokenize(command);
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens[0].is_right_bracket()) {
            close_else();
        }
        // Add command to group
        else {
            commandBuffer_.push_back(PoolString<StringPool>(stringPool_, command.c_str()));
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
            Log.trace(F("Else commands will be executed\n"));
            // Add commands to commandQueue in reverse order,
            // since pushing from the front
            for (int i = commandBuffer_.size() - 1; i >= 0; --i) {
                commandQueue_.push_front(commandBuffer_[i]);
                //stringPool_.inc_ref_count(commandBuffer_[i]);
                // Replaces
                //commandQueue_.push_front(std::string(stringPool_.c_str(commandBuffer_[i])));
            }
            // Push for any commands that might require in nested scope
            lastIfCondition_.push_back(-1);
            toPopLastIfCondition_.push_back(true);
        }
        decrease_nested_level();
        Log.trace(F("Closing else\n"));
    }

    /*!
        @brief  Begins the creation of a command group.

        @param  name
                The name of the command group to be created.
    */
    void create_group(std::string const & name) {
        increase_nested_level(InterpreterStatus::CREATING_GROUP);
        commandBuffer_.push_back(PoolString<StringPool>(stringPool_, name.c_str()));        
        //commandBuffer_.push_back(stringPool_.allocate_idx());
        //stringPool_.strcpy(commandBuffer_.back(), name.c_str());
        Log.trace(F("Creating group\n"));
    }

    /*!
        @brief  Adds a command to the command group that is currently being created.

        @param  command
                The command to add to the command group.
    */
    void add_to_group(std::string const & command) {
        std::vector<Token> tokens = tokenizer_.tokenize(command);
        // Group is closed
        if (bracketParity_ == 0 && tokens.size() == 2 && tokens[0].is_right_bracket()) {
            std::vector<std::string> commands;
            for (int i = 0; i < commandBuffer_.size(); ++i) {
                commands.push_back(std::string(commandBuffer_[i].c_str()));
            }
            std::string name = commands[0];
            commands.erase(commands.begin()); // Remove name
            close_group(name, commands);
        }
        // Add command to group
        else {
            commandBuffer_.push_back(PoolString<StringPool>(stringPool_, command.c_str()));
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
        @brief  Finishes the creation of a command group.

        @param  name
                The name of the command group to be created.
        
        @param  commands
                The commands that make up the command group.
    */
    void close_group(std::string const & name, std::vector<std::string> & commands) {
        Device group(DeviceType::GROUP);
        group.name = name;
        group.info = commands;
        devices_[name] = group;
        // Push for any commands that might require in nested scope
        lastIfCondition_.push_back(-1);
        toPopLastIfCondition_.push_back(true);
        decrease_nested_level();
        Log.trace(F("Closing group: %s\n"), name.c_str());
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
    StringPool& stringPool_;

    Deque<PoolString<StringPool>, Alloc> commandQueue_;
    Deque<PoolString<StringPool>, Alloc> commandBuffer_;

    std::map<std::string, Device> devices_;

    InterpreterStatus status_;

    /** Used to check for else/elseif blocks */
    Deque<int, Alloc> lastIfCondition_;
    Deque<int, Alloc> toPopLastIfCondition_;
    int bracketParity_;

    Parser parser_;
    Tokenizer tokenizer_;

};

} // namespace kty