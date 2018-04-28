#pragma once

#include <kitty/stl_impl.hpp>
#include <map>
#include <queue>
#include <stack>
#include <vector>
#include <Servo.h>

#include <kitty/tokenizer/tokenizer.hpp>
#include <kitty/parser/parser.hpp>

namespace kitty {

namespace interpreter {

/** The various device types possible */
enum DeviceType {
    NUM,
    LED, SERVO, GROUP,
    UNKNOWN,
};

/*!
    @brief  Converts a device type to its string equivalent

    @param  deviceType
            The device type to be converted
    
    @return The string version of the given device type.
            If the device type given is invalid, "UNKNOWN" is returned.
*/
std::string device_type_to_str(DeviceType deviceType) {
    static const std::map<DeviceType, std::string> lookupMap = {
        {NUM, "NUM"},
        {LED, "LED"},
        {SERVO, "SERVO"},
        {GROUP, "GROUP"},
    };
    if (lookupMap.find(deviceType) == lookupMap.end()) {
        return "UNKNOWN";
    }
    return lookupMap[deviceType];
}

/*!
    @brief  Class that holds information about a device
*/
struct Device {
    
    DeviceType type;                ///> The type of the device
    std::string name;               ///> The name of the device
    std::vector<std::string> info;  ///> A vector containing information about the device. This varies depending on the type of the device.

    Servo servo;                    ///> A servo object to be used if the device is a servo.

    /*!
        @brief  Constructor for device

        @param  _type
                The type of the device.
    */
    explicit Device(DeviceType _type = DeviceType::UNKNOWN) : type(_type) {
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
        if (type == DeviceType::UNKNOWN) {
            return "UNKNOWN";
        }
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
        };
        return str;
    }
};

/*!
    @brief  Class that stores state on all devices and groups, and executes commands.
*/
class Interpreter {

public:
    /*!
        @brief  Executes a given command.

        @param  command
                The command to execute.
                Tokens in command are assumed to be in postfix notation.
    */
    void execute(std::vector<tokenizer::Token> const & command) {
        // Nothing to execute
        if (command.empty()) {
            return;
        }
        if (command[0].type == tokenizer::TokenType::ELSE) {
            execute_else(command);
        }
        else if (command[0].type == tokenizer::TokenType::NAME) {
            // Creation of a device/group
            if (command.back().type == tokenizer::TokenType::CREATE_NUM || 
                command.back().type == tokenizer::TokenType::CREATE_LED || 
                command.back().type == tokenizer::TokenType::CREATE_SERVO || 
                command.back().type == tokenizer::TokenType::CREATE_GROUP) {
                execute_create(command);
            }
            // Running group
            else {

            }
        }
    }

    /*!
        @brief  Executes the else command.

        @param  command
                The command to execute.
    */
    void execute_else(std::vector<tokenizer::Token> const & command) {

    }

    /*!
        @brief  Executes the different creation commands.

        @param  command
                The command to execute.
    */
    void execute_create(std::vector<tokenizer::Token> const & command) {
        std::queue<tokenizer::Token> tokenQueue;
        for (int i = 0; i < command.size() - 1; ++i) {
            tokenQueue.push(command[i]);
        }
        std::string name = tokenQueue.front().value;
        tokenizer::Token createToken = command[command.size() - 1];
        tokenQueue.pop();

        std::stack<tokenizer::Token> result = evaluate_postfix(tokenQueue);

        if (createToken.type == tokenizer::TokenType::CREATE_NUM) {
            create_number(name, result);
        }
        else if (createToken.type == tokenizer::TokenType::CREATE_LED) {
            create_led(name, result);
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
            return Device(DeviceType::UNKNOWN);
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
    std::stack<tokenizer::Token> evaluate_postfix(std::queue<tokenizer::Token> & tokenQueue) {
        std::stack<tokenizer::Token> tokenStack;

        while (!tokenQueue.empty()) {
            tokenizer::Token token = tokenQueue.front();
            tokenQueue.pop();
            if (tokenizer::is_operator(token)) {
                tokenizer::Token lhs = tokenStack.top();
                tokenStack.pop();
                tokenizer::Token rhs = tokenStack.top();
                tokenStack.pop();
                tokenizer::Token result = evaluate_operation(token, lhs, rhs);
                tokenStack.push(result);
            }
            else if (tokenizer::is_operand(token)) {
                tokenStack.push(token);
            }
            else if (tokenizer::is_function(token)) {
                tokenStack.push(token);
            }
        }
        return tokenStack;
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
                an unknown token is returned.
    */
    tokenizer::Token evaluate_operation(tokenizer::Token const & op, tokenizer::Token const & lhs, tokenizer::Token const & rhs) {
        return tokenizer::Token(tokenizer::TokenType::UNKNOWN);
    }

    /*!
        @brief  Creates a number using the name and information given.

        @param  name
                The name of the number to be created
        
        @param  info
                The information about the number.
                The number value is expected to be the top token of the stack.
    */
    void create_number(std::string const & name, std::stack<tokenizer::Token> & info) {
        Serial.println("Creating number");
        Device number(DeviceType::NUM);
        number.name = name;
        number.info.push_back(info.top().value);
        devices_[name] = number;
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
    void create_led(std::string const & name, std::stack<tokenizer::Token> & info) {
        Serial.println("Creating LED");
        Device number(DeviceType::LED);
        number.name = name;
        int brightness = utils::str_to_int(info.top().value);
        info.pop();
        int pinNumber = utils::str_to_int(info.top().value);
        pinMode(pinNumber, OUTPUT);
        analogWrite(pinNumber, (int)(brightness * 2.55));
        number.info.push_back(utils::int_to_str(pinNumber));
        number.info.push_back(utils::int_to_str(brightness));
        devices_[name] = number;
    }

private:
    std::queue<std::string> commandQueue_;
    std::map<std::string, Device> devices_;

};

} // namespace interpreter

} // namespace kitty