#pragma once

#ifndef KITTY_INTERPRETER_HPP_
#define KITTY_INTERPRETER_HPP_

#include <StandardCplusplus.h>

#include <cctype>
#include <map>
#include <deque>
#include <sstream>
#include <string>
#include <vector>

#include <Regexp.h>

#include <kitty/commands/kitty_commands.hpp>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/variables/kitty_variables.hpp>

namespace kitty {

using std::istringstream;
using std::map;
using std::deque;
using std::string;
using std::vector;

class kitty_interpreter {
public:
    kitty_interpreter() 
            : isPreloadedMode(false) {}

    void print_welcome_screen() {
        Serial.begin(115200);            
        Serial.println(F(R"(
            _                        
            \`*-.                    
             )  _`-.                 
            .  : \  .                
            : ^   '  \               
            ; *  _.   `*-._          
            `-.-'          `-.       
              ;       `       `.     
              :.       .        \    
             ,  \   ;  :   .-'   .   
            .  , `..;  ;  '      :   _.--.
 ,-.       '  .     |  , ;       ;.-'_.-'`
:       .*'  .      : :`-:     _.`-'`
`      `*-*      .*' ; .*`- +' 
 '.(bug)          `*-*  `*-*'        
        )"));
        Serial.println(F("Welcome to Kitty 0.0.1 (by Mattheus Lee, mattheus.lee@gmail.com, 2017-)"));
    }

    void print_prompt() {
        Serial.print(F(">>>"));        
    }

    void execute(string const & input) {
        commandDeque_.push_back(input);
        while (!commandDeque_.empty()) {
            parse_and_execute_single_command_();
        }
    }

    void execute_preloaded_commands(vector<string> const & commands) {
        Serial.println(F("Kitty interpreter is in preloaded commands mode"));
        isPreloadedMode = true;
        for (auto const & command : commands) {
            commandDeque_.push_back(command);
        }
        while (!commandDeque_.empty()) {
            parse_and_execute_single_command_();
        }
        Serial.println(F("Program done"));
    }

private:
    template <class T1, class T2>
    struct pair_ {
        T1 first;
        T2 second;
    };

    template <class T1, class T2>
    pair_<T1, T2> make_pair_(T1 const & t1, T2 const & t2) {
        return pair_<T1, T2>{t1, t2};
    }

    bool isPreloadedMode;
    map<string, kitty_variable> variables_;
    map<string, pair_<string, vector<string>>> commandGroups_;
    deque<string> commandDeque_;
    kitty_storage storage_;

    void parse_and_execute_single_command_() {
        string command = commandDeque_.front();
        commandDeque_.pop_front();        
        MatchState matchState;
        matchState.Target(command.c_str());
        if (kitty_device_variable_command::matches(matchState)) {
            kitty_device_variable_command::execute(matchState, storage_);
        } else if (kitty_numeric_variable_command::matches(matchState)) {
            kitty_numeric_variable_command::execute(matchState, storage_);
        }
        // Increase number by value or variable
        else if (matchState.Match("^increase ([%a_]+) by ([%d%a_]+)$") > 0) {
            increase_by(matchState);
        }
        // Increase number
        else if (matchState.Match("^increase ([%a_]+)$")) {
            increase_by(matchState, 1);
            return;
        }
        // Decrease number by value or variable
        else if (matchState.Match("^decrease ([%a_]+) by ([%d%a_]+)$") > 0) {
            decrease_by_(matchState);
        }
        // Decrease number
        else if (matchState.Match("^decrease ([%a_]+)$")) {
            decrease_by_(matchState, 1);
            return;
        }
        // Sense from device
        else if (matchState.Match("^sense ([%a_]+) from ([%a_]+)$") > 0) {
            sense_from_device_(matchState);
            return;
        }
        // Move device by value
        else if (matchState.Match("^move ([%a_]+) by ([%d-]+)$") > 0) {
            move_device_value_(matchState, "by");
            return;
        }
        // Move device by variable
        else if (matchState.Match("^move ([%a_]+) by ([%a_]+)$") > 0) {
            move_device_variable_(matchState, "by");
            return;
        }
        // Move device to value
        else if (matchState.Match("^move ([%a_]+) to ([%d-]+)$") > 0) {
            move_device_value_(matchState, "to");
            return;
        }
        // Move device to variable
        else if (matchState.Match("^move ([%a_]+) to ([%a_]+)$") > 0) {
            move_device_variable_(matchState, "to");
            return;
        }
        // Create group of commands with if condition
        else if (matchState.Match("^create ([%a_]+) if ([%a%d%+%-%(%)_ ]+)$") > 0) {
            create_if_group_(matchState);
            return;
        }
        // Create group of commands with while condition
        else if (matchState.Match("^create ([%a_]+) while ([%a%d%+%-%(%)_ ]+)$") > 0) {
            create_while_group_(matchState);
            return;
        }
        // Create group of commands that automatically repeats
        else if (matchState.Match("^create ([%a_]+) repeating$") > 0) {
            create_repeating_group_(matchState);
            return;
        }
        // Create group of commands
        else if (matchState.Match("^create ([%a_]+)$") > 0) {
            create_group_(matchState);
            return;
        }
        // Run group of commands
        else if (matchState.Match("^run ([%a_]+)$") > 0) {
            run_group_(matchState);
            return;
        }
        // Wait amount of time
        else if (matchState.Match("^wait ([%d]+)m?s$") > 0) {
            wait_(matchState);
            return;
        }
        else if (kitty_print_information_command::matches(matchState)) {
            kitty_print_information_command::execute(matchState, storage_);
        }
        else {
            Serial.println(F("ERROR: unable to parse line"));
        }
    }

    bool evaluate_condition_(string const & ifCondition) {
        if (!match_brackets_(ifCondition)) {
            return false;
        }
        if (evaluate_condition_helper_(ifCondition)) {
            //Serial.print(F("Evaluated >>>"));
            //Serial.print(ifCondition.c_str());
            //Serial.println(F("<<<to true"));        
            return true;
        } else {
            //Serial.print(F("Evaluated >>>"));
            //Serial.print(ifCondition.c_str());
            //Serial.println(F("<<< to false")); 
            return false;
        }
    }

    bool evaluate_condition_helper_(string const & condition) {
        MatchState matchState;
        // Don't trim brackets for now, settle unmatched brackets at the bottom levels
        matchState.Target(condition.c_str());
        // True literal
        if (matchState.Match("^%(*true%(*$") > 0) {
            return true;
        }
        // False literal
        else if (matchState.Match("^%(*false%(*$") > 0) {
            return false;
        }
        // Less than check
        else if (matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) less than ([%a%d%+%-%(%)_ ]+)%)*$") > 0) {
            //Serial.println(F("less check"));
            auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
            auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
            //Serial.println((string("") + "lhs " + lhs).c_str());
            //Serial.println((string("") + "rhs " + rhs).c_str());
            auto evaluatedLhs = evaluate_value_(lhs);
            auto evaluatedRhs = evaluate_value_(rhs);
            if (!evaluatedLhs.first || !evaluatedRhs.first) {
                return false;
            }
            return evaluatedLhs.second < evaluatedRhs.second;
        }
        // Greater than check
        else if (matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) greater than ([%a%d%+%-%(%)_ ]+)%)*$") > 0) {
            //Serial.println(F("greater check"));
            auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
            auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
            //Serial.println((string("") + "lhs " + lhs).c_str());
            //Serial.println((string("") + "rhs " + rhs).c_str());
            auto evaluatedLhs = evaluate_value_(lhs);
            auto evaluatedRhs = evaluate_value_(rhs);
            if (!evaluatedLhs.first || !evaluatedRhs.first) {
                return false;
            }
            return evaluatedLhs.second > evaluatedRhs.second;
        }
        // Equals check
        else if (matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) equals ([%a%d%+%-%(%)_ ]+)%)*$") > 0) {
            //Serial.println(F("equals check"));
            auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
            auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
            //Serial.println((string("") + "lhs " + lhs).c_str());
            //Serial.println((string("") + "rhs " + rhs).c_str());
            auto evaluatedLhs = evaluate_value_(lhs);
            auto evaluatedRhs = evaluate_value_(rhs);
            if (!evaluatedLhs.first || !evaluatedRhs.first) {
                return false;
            }
            return evaluatedLhs.second == evaluatedRhs.second;
        }
        Serial.print(F("ERROR: unable to evaluate condition "));
        Serial.println(condition.c_str());
        return false;
    }

    pair_<bool, int> evaluate_value_(string const & arg) {
        MatchState matchState;
        matchState.Target(arg.c_str());
        // Addition subexpression
        if (matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %+ ([%a%d%+%-%(%)_ ]+)%)*$")) {
            //Serial.println("Addition subexpression");
            auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
            auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
            //Serial.println((string("") + "lhs " + lhs).c_str());
            //Serial.println((string("") + "rhs " + rhs).c_str());
            auto evaluatedLhs = evaluate_value_(lhs);
            auto evaluatedRhs = evaluate_value_(rhs);
            if (!evaluatedLhs.first || !evaluatedRhs.first) {
                return make_pair_(false, 0);
            }
            return make_pair_(true, evaluatedLhs.second + evaluatedRhs.second);
        }
        // Subtraction subexpression
        else if (matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %- ([%a%d%+%-%(%)_ ]+)%)*$")) {
            //Serial.println("Subtraction subexpression");
            auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
            auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
            //Serial.println((string("") + "lhs " + lhs).c_str());
            //Serial.println((string("") + "rhs " + rhs).c_str());
            auto evaluatedLhs = evaluate_value_(lhs);
            auto evaluatedRhs = evaluate_value_(rhs);
            if (!evaluatedLhs.first || !evaluatedRhs.first) {
                return make_pair_(false, 0);
            }
            return make_pair_(true, evaluatedLhs.second - evaluatedRhs.second);
        }
        // Single variable subexpression
        else if (matchState.Match("^%(*([%a_]+)%)*$")) {
            //Serial.println("Single variable subexpression");
            auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
            if (!find_variable_(varName) || !check_is_value_(varName)) {
                return make_pair_(false, 0);
            }
            return make_pair_(true, variables_[varName].get_value());
        }
        // Single digit subexpression
        else if (matchState.Match("^%(*(%-?%d+)%)*$")) {
            //Serial.println("Single digit subexpression");
            return make_pair_(true, to_int_(string(matchState.capture[0].init, matchState.capture[0].len)));
        }
        return make_pair_(false, 0);
    }

    int to_int_(string const & str) {
        istringstream iss(str);
        auto isNegativeValue = false;
        if (iss.str()[0] == '-') {
            isNegativeValue = true;
            char c;
            iss >> c;
        }
        auto value = 0;
        iss >> value;
        if (isNegativeValue) {
            value *= -1;
        }
        return value;
    }

    bool match_brackets_(string const & statement) {
        int bracketCount = 0;
        for (auto const & c : statement) {
            if (c == '(') {
                ++bracketCount;
            } else if (c == ')') {
                --bracketCount;
            }
        }
        if (bracketCount != 0) {
            Serial.print(F("ERROR: "));
            Serial.print(statement.c_str());
            Serial.println(F(" has mismatched brackets"));
            return false;
        }
        return true;
    }

    string trim_outer_brackets_(string const & statement) {
        if (statement[0] == '(') {
            return statement.substr(1, statement.size() - 1);
        }
        return statement;
    }

    string get_line_() {
        string line;
        while (1) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n') {
                    break;
                }
                line += c;
            }
        }
        if (line[line.size() - 1] == ' ') {
            line.erase(line.size() - 1);
        }
        return line;
    }

    bool find_variable_(string const & varName) {
        if (!check_find_variable_(varName)) {
            Serial.print(F("ERROR: "));
            Serial.print(varName.c_str());
            Serial.println(F(" does not exist"));
            return false;
        }
        return true;
    }

    bool check_find_variable_(string const & varName) {
        return variables_.find(varName) != variables_.end();
    }

    bool find_command_group_(string const & groupName) {
        if (!check_find_command_group_(groupName)) {
            Serial.print(F("ERROR: "));
            Serial.print(groupName.c_str());
            Serial.println(F(" does not exist"));
            return false;
        }
        return true;
    }

    bool check_find_command_group_(string const & groupName) {
        return commandGroups_.find(groupName) != commandGroups_.end();
    }

    bool check_sensor_(string const & devName) {
        if (!variables_[devName].is_sensor()) {
            Serial.print(F("ERROR: "));
            Serial.print(devName.c_str());
            Serial.println(F(" is not a sensor -"));
            Serial.print(devName.c_str());
            Serial.print(F(": "));
            Serial.print(variables_[devName].str().c_str());
            return false;
        }
        return true;
    }

    bool check_mover_(string const & devName) {
        if (!variables_[devName].is_mover()) {
            Serial.print(F("ERROR: "));
            Serial.print(devName.c_str());
            Serial.println(F(" cannot be moved -"));
            Serial.print(devName.c_str());
            Serial.print(F(": "));
            Serial.print(variables_[devName].str().c_str());
            return false;
        }
        return true;
    }

    bool check_is_value_(string const & varName) {
        if (!variables_[varName].is_value()) {
            Serial.print(F("ERROR: "));
            Serial.print(varName.c_str());
            Serial.println(F(" is not a value -"));
            Serial.print(varName.c_str());
            Serial.print(F(": "));
            Serial.print(variables_[varName].str().c_str());
            return false;
        }
        return true;
    }

    void increase_by(MatchState const & matchState) {
        auto arg = string(matchState.capture[1].init, matchState.capture[1].len);
        auto increment = 0;
        if (isdigit(arg[0])) {
            increment = to_int_(arg);
        } else {
            if (!find_variable_(arg) || !check_is_value_(arg)) {
                return;
            }
            increment = variables_[arg].get_value();
        }
        increase_by(matchState, increment);
    }

    void increase_by(MatchState const & matchState, int const & increment) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!find_variable_(varName) || !check_is_value_(varName)) {
            return;
        }        
        auto value = variables_[varName].get_value();
        variables_[varName].set(kitty_int(value + increment));
    }

    void decrease_by_(MatchState const & matchState) {
        auto arg = string(matchState.capture[1].init, matchState.capture[1].len);
        auto decrement = 0;
        if (isdigit(arg[0])) {
            decrement = to_int_(arg);
        } else {
            if (!find_variable_(arg) || !check_is_value_(arg)) {
                return;
            }
            decrement = variables_[arg].get_value();
        }
        decrease_by_(matchState, decrement);
    }

    void decrease_by_(MatchState const & matchState, int const & decrement) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!find_variable_(varName) || !check_is_value_(varName)) {
            return;
        }        
        auto value = variables_[varName].get_value();
        variables_[varName].set(kitty_int(value - decrement));
    }

    void sense_from_device_(MatchState const & matchState) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto args = string(matchState.capture[1].init, matchState.capture[1].len);
        auto devName = string();
        auto type = string();
        if (args.find(" ") != string::npos) { // Type of sensing is specified
            devName = args.substr(0, args.find(" "));
            type = args.substr(args.find(" ") + 1);
        } else {
            devName = args;
        }
        if (!find_variable_(devName) || !check_sensor_(devName)) {
            return;
        }
        auto val = variables_[devName].sense(type);
        variables_[varName].set(kitty_int(val));
    }

    void move_device_value_(MatchState const & matchState, string const & type) {
        auto devName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto value = to_int_(string(matchState.capture[1].init, matchState.capture[1].len));
        if (!find_variable_(devName) || !check_mover_(devName)) {
            return;
        }
        if (type == "by") {
            variables_[devName].move_by(value);
        } else {
            variables_[devName].move_to(value);
        }
    }

    void move_device_variable_(MatchState const & matchState, string const & type) {
        auto devName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto varName = string(matchState.capture[1].init, matchState.capture[1].len);
        if (!find_variable_(devName) || !check_mover_(devName) || !find_variable_(varName) || !check_is_value_(varName)) {
            return;
        }
        if (type == "by") {
            variables_[devName].move_by(variables_[varName].get_value());
        } else {
            variables_[devName].move_to(variables_[varName].get_value());
        }
    }

    vector<string> get_group_commands_(string const & groupName) {
        auto groupCommands = vector<string>();
        if (isPreloadedMode) {
            while(commandDeque_.front() != "done") {
                groupCommands.push_back(commandDeque_.front());
                commandDeque_.pop_front();
            }
            commandDeque_.pop_front();
        } else {
            Serial.print(groupName.c_str());
            Serial.print(F(">>>"));
            auto inputLine = get_line_();
            while (inputLine != "done") {
                groupCommands.push_back(inputLine);
                Serial.print(inputLine.c_str());
                Serial.println(F(" added to the group"));
                Serial.print(groupName.c_str());
                Serial.print(F(">>>"));
                inputLine = get_line_();
            }
            Serial.println(F("group complete"));
        }
        return groupCommands;
    }

    void create_if_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto ifCondition = string(matchState.capture[1].init, matchState.capture[1].len);
        if (!match_brackets_(ifCondition)) {
            return;
        }
        auto groupCommands = get_group_commands_(groupName);
        commandGroups_[groupName] = make_pair_(ifCondition, groupCommands);
    }

    void create_while_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto whileCondition = string(matchState.capture[1].init, matchState.capture[1].len);
        if (!match_brackets_(whileCondition)) {
            return;
        }
        auto groupCommands = get_group_commands_(groupName);
        groupCommands.push_back("run " + groupName);
        commandGroups_[groupName] = make_pair_(whileCondition, groupCommands);
    }

    void create_repeating_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto groupCommands = get_group_commands_(groupName);
        groupCommands.push_back("run " + groupName);
        commandGroups_[groupName] = make_pair_(string("true"), groupCommands);        
    }

    void create_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto groupCommands = get_group_commands_(groupName);
        commandGroups_[groupName] = make_pair_(string("true"), groupCommands);
    }

    void run_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!find_command_group_(groupName) || !evaluate_condition_(commandGroups_[groupName].first)) {
            return;
        }
        for (auto it = commandGroups_[groupName].second.rbegin(); it != commandGroups_[groupName].second.rend(); ++it) {
            commandDeque_.push_front(*it);
        }
    }

    void wait_(MatchState const & matchState) {
        auto timeToWait = to_int_(string(matchState.capture[0].init, matchState.capture[0].len));
        if (matchState.Match("[%d]+ms")) {
            delay(timeToWait);
        } else {
            delay(timeToWait * 1000);
        }
    }

protected:

};

} // kitty

#endif // KITTY_INTERPRETER_HPP_
