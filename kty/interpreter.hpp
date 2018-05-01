#pragma once

#include <kty/stl_impl.hpp>
#include <map>
#include <queue>
#include <stack>
#include <vector>
#if defined(ARDUINO)
#include <Servo.h>
#endif

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
    static const std::map<DeviceType, std::string> lookupMap = {
        {NUM, "NUM"},
        {LED, "LED"},
        {SERVO, "SERVO"},
        {GROUP, "GROUP"},
    };
    if (lookupMap.find(deviceType) == lookupMap.end()) {
        return "UNKNOWN_DEVICE";
    }
    return lookupMap.find(deviceType)->second;
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
            str += "Command Group containing the command(s):\n";
            for (int i = 0; i < info.size(); ++i) {
                 str += "    " + info[i] + "\n";
            }
            break;
        case UNKNOWN_DEVICE:
        default:
            return "UNKNOWN_DEVICE";            
        };
        return str;
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
    CREATING_GROUP,
    CREATING_IF,
    CREATING_ELSE,
    CREATING_ELSEIF,
    NORMAL,
};

/*!
    @brief  Class that stores state on all devices and groups, and executes commands.
*/
class Interpreter {

public:
    /*!
        @brief  Default interpreter constructor.
    */
    Interpreter() {
        status_.push(InterpreterStatus::NORMAL);
    }

    /*!
        @brief  Gets the prefix(if any) for the user prompt.

        @return The prefix for the user prompt.
                If there is no required prefix, an empty string is returned.
    */
    std::string get_prompt_prefix() const {
        switch (status_.top()) {
        case NORMAL:
            return "";
        case CREATING_GROUP:
            return "(" + commandBuffer_.top()[0] + ")";
        default:
            return "";
        }
    }

    /*!
        @brief  Executes a given command.

        @param  command
                The raw string command to execute.
    */
    void execute(std::string const & command) {
        // Nothing to execute
        if (command.size() == 0) {
            return;
        }
        std::vector<Token> tokens;
        switch (status_.top()) {
        // Normal execution
        case NORMAL:
            tokens = tokenizer_.tokenize(command);
            tokens = parser_.parse(tokens);
            execute(tokens);
            break;
        case CREATING_GROUP:
            add_to_group(command);
        };
        // Execute command queue, if any
        execute_command_queue();
    }

    /*!
        @brief  Executes all the commands still in the command queue, if any.
    */
    void execute_command_queue() {
        // No commands to execute
        if (commandQueue_.size() == 0) {
            return;
        }
        while (!commandQueue_.empty()) {
            std::string command = commandQueue_.front();
            commandQueue_.pop();
            Serial.print("Executing = ");
            Serial.println(command.c_str());
            execute(command);
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
        if (command[0].type == TokenType::ELSE) {
            execute_else(command);
        }
        else if (command[0].type == TokenType::NAME) {
            // Creation of a device/group
            if (command.back().type == TokenType::CREATE_NUM || 
                command.back().type == TokenType::CREATE_LED || 
                command.back().type == TokenType::CREATE_SERVO || 
                command.back().type == TokenType::CREATE_GROUP) {
                execute_create(command);
            }
            // Running group
            else {
                execute_run_group(command);
            }
        }
    }

    /*!
        @brief  Executes the else command.

        @param  command
                The command to execute.
    */
    void execute_else(std::vector<Token> const & command) {

    }

    /*!
        @brief  Executes the different creation commands.

        @param  command
                The command to execute.
    */
    void execute_create(std::vector<Token> const & command) {
        std::queue<Token> tokenQueue;
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
        else if (createToken.is_create_servo()) {
            
        }
        else if (createToken.is_create_group()) {
            create_group(name);
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
        Serial.print("Running group = ");
        Serial.println(name.c_str());
        for (int i = 0; i < numTimes; ++i) {
            for (int j = 0; j < commands.size(); ++j) {
                Serial.println(commands[j].c_str());
                commandQueue_.push(commands[j]);
            }
        }
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
            }
            else if (token.is_operator()) {
                Token rhs = tokenStack.top();
                tokenStack.pop();
                Token lhs = tokenStack.top();
                tokenStack.pop();
                Token result = evaluate_operation(token, lhs, rhs);
                //Serial.print(lhs.str().c_str());
                //Serial.print(token.str().c_str());
                //Serial.print(rhs.str().c_str());
                //Serial.print(" = ");
                //Serial.println(result.str().c_str());
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
        Serial.print("Creating number = ");
        Serial.println(info.top().value.c_str());
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
        Serial.println("Creating LED");
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
    }

    /*!
        @brief  Begins the creation of a command group.

        @param  name
                The name of the command group to be created.
    */
    void create_group(std::string const & name) {
        status_.push(InterpreterStatus::CREATING_GROUP);
        commandBuffer_.push(std::vector<std::string>());
        commandBuffer_.top().push_back(name);
        Serial.print("Creating group = ");
        Serial.println(name.c_str());
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
        status_.pop();
        Serial.print("Closing group = ");
        Serial.println(name.c_str());
    }

    /*!
        @brief  Adds a command to the command group that is currently being created.

        @param  command
                The command to add to the command group.
    */
    void add_to_group(std::string const & command) {
        std::vector<Token> tokens = tokenizer_.tokenize(command);
        // Check if the group is complete
        if (tokens.size() == 2 && tokens[0].is_right_bracket()) {
            std::vector<std::string> commands = commandBuffer_.top();
            commandBuffer_.pop();
            std::string name = commands[0];
            commands.erase(commands.begin());
            close_group(name, commands);
        }
        // Add command to group
        else {
            commandBuffer_.top().push_back(command);
            Serial.print("Adding command to group = ");
            Serial.println(command.c_str());
        }
    }

private:
    std::queue<std::string> commandQueue_;
    std::map<std::string, Device> devices_; 

    std::stack<InterpreterStatus> status_;
    std::stack<std::vector<std::string>> commandBuffer_;

    Parser parser_;
    Tokenizer tokenizer_;

};

} // namespace kty