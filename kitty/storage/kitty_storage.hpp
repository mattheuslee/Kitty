#pragma once

#ifndef KITTY_STORAGE_HPP_
#define KITTY_STORAGE_HPP_

#include <StandardCplusplus.h>

#include <deque>
#include <map>
#include <string>
#include <vector>

#include <kitty/variables/kitty_variable.hpp>

namespace kitty {

using std::deque;
using std::map;
using std::string;
using std::vector;

class kitty_storage {

public:
    kitty_storage()
            : isLiveMode_(true) {}

    bool is_live_mode() {
        return isLiveMode_;
    }

    void is_live_mode(bool const & isLiveMode) {
        isLiveMode_ = isLiveMode;
    }

    void print_info(string const & name) const {
        if (check_variable_exists(name)) {
            Serial.print(name.c_str());
            Serial.print(F(": "));
            Serial.println(variables_[name].str().c_str());
        } else if (assert_command_group_exists(name)) {
            Serial.println((name + ": command group with commands").c_str());
            for (auto const & command : commandGroups_[name]) {
                Serial.println(command.c_str());
            }
        }
    }

    void print(string const & name) const {
        if (assert_variable_exists(name)) {
            variables_[name].print();
        }
    }

    bool assert_variable_exists(string const & varName) const {
        if (!check_variable_exists(varName)) {
            print_does_not_exist_(varName);
            return false;
        }
        return true;
    }

    bool check_variable_exists(string const & varName) const {
        return variables_.find(varName) != variables_.end();
    }

    bool assert_command_group_exists(string const & groupName) const {
        if (!check_command_group_exists(groupName)) {
            print_does_not_exist_(groupName);
            return false;
        }
        return true;
    }

    bool check_command_group_exists(string const & groupName) const {
        return commandGroups_.find(groupName) != commandGroups_.end();
    }

    bool assert_variable_is_value(string const & varName) const {
        if (!check_variable_is_value(varName)) {
            print_is_not_a_(varName, "value");
            print_info(varName);
            return false;
        }
        return true;
    }

    bool check_variable_is_value(string const & varName) const {
        return variables_[varName].is_value();
    }

    bool assert_variable_is_mover(string const & varName) const {
        if (!check_variable_is_mover(varName)) {
            print_is_not_a_(varName, "mover");
            print_info(varName);
            return false;
        }
        return true;
    }

    bool check_variable_is_mover(string const & varName) const {
        return variables_[varName].is_mover();
    }

    bool assert_variable_is_sensor(string const & varName) const {
        if (!check_variable_is_sensor(varName)) {
            print_is_not_a_(varName, "sensor");
            print_info(varName);
            return false;
        }
        return true;
    }

    bool check_variable_is_sensor(string const & varName) const {
        return variables_[varName].is_sensor();
    }

    kitty_variable& variable(string const & varName) {
        return variables_[varName];
    }

    vector<string>& command_group(string const & groupName) {
        return commandGroups_[groupName];
    }

    string get_next_command() {
        auto nextCommand = peek_next_command();
        if (!commandDeque_.empty()) {
            commandDeque_.pop_front();
        }
        return nextCommand;
    }

    bool has_more_commands() {
        return !commandDeque_.empty();
    }

    string pop_next_command() {
        return get_next_command();
    }

    string peek_next_command() const {
        auto nextCommand = string();
        if (!commandDeque_.empty()) {
            nextCommand = commandDeque_.front();
        }
        return nextCommand;
    }

    void add_command_to_front(string const & command) {
        commandDeque_.push_front(command);
    }

    void add_command_to_back(string const & command) {
        commandDeque_.push_back(command);
    }

private:
    bool isLiveMode_;
    map<string, kitty_variable> variables_;
    map<string, vector<string>> commandGroups_;
    deque<string> commandDeque_;

    void print_is_not_a_(string const & varName, string const & typeName) {
        Serial.print(F("ERROR: "));
        Serial.print(varName.c_str());
        Serial.print(F(" is not a "));
        Serial.println(typeName.c_str());
    }

    void print_does_not_exist_(string const & name) {
        Serial.print(F("ERROR: "));
        Serial.print(name.c_str());
        Serial.println(F(" does not exist"));
    }

protected:

};

}

#endif // KITTY_STORAGE_HPP_
