#pragma once

#ifndef KITTY_STORAGE_HPP_
#define KITTY_STORAGE_HPP_

#include <StandardCplusplus.h>

#include <deque>
#include <map>
#include <string>
#include <vector>

#include <kitty/utility/kitty_pair.hpp>
#include <kitty/variables/kitty_variable.hpp>

namespace kitty {

using std::deque;
using std::map;
using std::string;
using std::vector;

class kitty_storage {

public:
    kitty_storage() {}

    void print_info(string const & name) const {
        if (check_variable_exists(name)) {
            Serial.print(name.c_str());
            Serial.print(F(": "));
            Serial.println(variables_[name].str().c_str());
        } else if (assert_command_group_exists(name)) {
            Serial.println((name + ": command group with condition " + commandGroups_[name].first + " and commands").c_str());
            for (auto const & command : commandGroups_[name].second) {
                Serial.println(command.c_str());
            }
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

    kitty_pair<string, vector<string>>& commandGroup(string const & groupName) {
        return commandGroups_[groupName];
    }

    string get_next_command() {
        auto nextCommand = peek_next_command();
        if (!commandDeque_.empty()) {
            commandDeque_.pop_front();
        }
        return nextCommand;
    }

    string peek_next_command() const {
        auto nextCommand = string();
        if (!commandDeque_.empty()) {
            nextCommand = commandDeque_.front();
        }
        return nextCommand;
    }

private:
    map<string, kitty_variable> variables_;
    map<string, kitty_pair<string, vector<string>>> commandGroups_;
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
