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

enum DeviceType {
    NUM,
    LED, SERVO, GROUP,
    UNKNOWN,
};

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

struct Device {
    DeviceType type;
    std::string name;
    std::vector<std::string> info;

    Servo servo;

    explicit Device(DeviceType _type = DeviceType::UNKNOWN) : type(_type) {
    }

    std::string debug_str() const {
        std::string str = "Device(" + device_type_to_str(type) + ")";
        return "Device(" + device_type_to_str(type) + ")";
    }

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

class Interpreter {

public:
    Interpreter() {
    }

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

    void execute_else(std::vector<tokenizer::Token> const & command) {

    }

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

    Device get_device(std::string const & name) {
        if (devices_.find(name) == devices_.end()) {
            return Device(DeviceType::UNKNOWN);
        }
        return devices_[name];
    }

    std::stack<tokenizer::Token> evaluate_postfix(std::queue<tokenizer::Token> & tokenQueue) {
        std::stack<tokenizer::Token> tokenStack;

        while (!tokenQueue.empty()) {
            tokenizer::Token token = tokenQueue.front();
            tokenQueue.pop();
            if (tokenizer::is_operator(token)) {
                tokenizer::Token operand1 = tokenStack.top();
                tokenStack.pop();
                tokenizer::Token operand2 = tokenStack.top();
                tokenStack.pop();
                tokenizer::Token result = evaluate_operation(token, operand1, operand2);
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

    tokenizer::Token evaluate_operation(tokenizer::Token const & op, tokenizer::Token const & operand1, tokenizer::Token const & operand2) {
        return tokenizer::Token(tokenizer::TokenType::UNKNOWN);
    }

    void create_number(std::string const & name, std::stack<tokenizer::Token> & info) {
        Serial.println("Creating number");
        Device number(DeviceType::NUM);
        number.name = name;
        number.info.push_back(info.top().value);
        devices_[name] = number;
    }

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