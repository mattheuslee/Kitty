#pragma once

#ifndef KITTY_INTERPRETER_HPP_
#define KITTY_INTERPRETER_HPP_

#include <StandardCplusplus.h>

#include <cctype>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include <Regexp.h>

#include <kitty_variable.hpp>
#include <kitty_int.hpp>
#include <kitty_led.hpp>
#include <kitty_servo.hpp>
#include <kitty_sonar.hpp>

namespace kitty {

using std::istringstream;
using std::map;
using std::queue;
using std::string;
using std::vector;

class kitty_interpreter {

public:
    kitty_interpreter() {}

    void execute(string const & input) {
        actionQueue_.push(make_pair_(input, false));
        while (!actionQueue_.empty()) {
            parse_and_execute_single_action_();
            actionQueue_.pop();
        }
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

    map<string, kitty_variable> variables_;
    map<string, vector<string>> actionGroups_;
    map<string, pair_<string, vector<string>>> actionIfGroups_;
    queue<pair_<string, bool>> actionQueue_;

    void parse_and_execute_single_action_() {
        string action = actionQueue_.front().first;
        if (actionQueue_.front().second) {
            //Serial.println((string("") + "Running >>>" + action + "<<<").c_str());
        }
        MatchState matchState;
        char result;
        matchState.Target(action.c_str());
        // Device variable
        if (matchState.Match("^([%a_]+) is ([%a_]+) using ([%d%a_, ]+)$") > 0) {
            device_variable_(matchState);
            return;
        }
        // Numeric variable
        else if (matchState.Match("^([%a_]+) is ([%a%d%+%-%(%)_ ]+)") > 0) {
            numeric_variable_(matchState);
            return;
        }
        // Viewing variable
        else if (matchState.Match("^([%a_]+)$") > 0) {
            viewing_variable_(matchState);
            return;
        }
        // Increment number by value or variable
        else if (matchState.Match("^increment ([%a_]+) by ([%d%a_]+)$") > 0) {
            increment_by_(matchState);
        }
        // Increment number
        else if (matchState.Match("^increment ([%a_]+)$")) {
            increment_by_(matchState, 1);
            return;
        }
        // Decrement number by value or variable
        else if (matchState.Match("^decrement ([%a_]+) by ([%d%a_]+)$") > 0) {
            decrement_by_(matchState);
        }
        // Decrement number
        else if (matchState.Match("^decrement ([%a_]+)$")) {
            decrement_by_(matchState, 1);
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
        // Create group of actions with if condition
        else if (matchState.Match("^create group ([%a_]+) if ([%a%d%+%-%(%)_ ]+)$") > 0) {
            create_if_group_(matchState);
            return;
        }
        // Create group of actions
        else if (matchState.Match("^create group ([%a_]+)$") > 0) {
            create_group_(matchState);
            return;
        }
        // Run group of actions
        else if (matchState.Match("^run group ([%a_]+)$") > 0) {
            run_group_(matchState);
            return;
        }
        // Wait amount of time
        else if (matchState.Match("^wait ([%d]+)m?s$") > 0) {
            wait_(matchState);
            return;
        }
        else {
            Serial.println(F("ERROR: unable to parse line"));
        }
    }

    bool evaluate_if_condition_(string const & ifCondition) {
        if (!match_brackets_(ifCondition)) {
            return false;
        }
        if (evaluate_condition_(ifCondition)) {
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

    bool evaluate_condition_(string const & condition) {
        MatchState matchState;
        // Don't trim brackets for now, settle unmatched brackets at the bottom levels
        matchState.Target(condition.c_str());
        // Less than check
        if (matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) less than ([%a%d%+%-%(%)_ ]+)%)*$") > 0) {
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
        else if (matchState.Match("^%(*(%d+)%)*$")) {
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
        return line;
    }

    bool find_variable_(string const & varName) {
        if (variables_.find(varName) == variables_.end()) {
            Serial.print(F("ERROR: "));
            Serial.print(varName.c_str());
            Serial.println(F(" does not exist"));
            return false;
        }
        return true;
    }

    bool find_action_group_(string const & groupName) {
        return actionGroups_.find(groupName) != actionGroups_.end();
    }

    bool find_action_if_group_(string const & groupName) {
        return actionIfGroups_.find(groupName) != actionIfGroups_.end();
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

    void device_variable_(MatchState const & matchState) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto deviceName = string(matchState.capture[1].init, matchState.capture[1].len);
        auto argList = string(matchState.capture[2].init, matchState.capture[2].len);
        MatchState argMatchState;
        argMatchState.Target(argList.c_str());
        auto numArgs = argMatchState.MatchCount("([%d%a_]+)");

        auto arguments = vector<int>();
        istringstream iss(argList);
        for (auto i = 0; i < numArgs; ++i) {
            if (isdigit(iss.peek())) {
                auto number = 0;
                iss >> number;
                arguments.push_back(number);
            } else {
                auto varName = string();
                char c = iss.get();
                do {
                    varName += c;
                    c = iss.get();
                } while (c != ',' && c != ' ' && c != EOF);
                if (!find_variable_(varName) || !check_is_value_(varName)) {
                    return;
                }
                arguments.push_back(variables_[varName].get_value());
            }
            while (iss.peek() == ' ' || iss.peek() == ',') {
                iss.get();
            }
        }
        if (deviceName == "servo") {
            if (numArgs != 1) {
                Serial.println(F("ERROR: wrong number of inputs for servo"));
            } else {
                variables_[varName].set(kitty_servo(arguments[0]));
            }
        } else if (deviceName == "led") {
            if (numArgs != 1) {
                Serial.println(F("ERROR: wrong number of inputs for LED"));
            } else {
                variables_[varName].set(kitty_led(arguments[0]));
            }
        } else if (deviceName == "sonar") {
            if (numArgs != 2) {
                Serial.println(F("ERROR: wrong number of inputs for sonar"));
            } else {
                variables_[varName].set(kitty_sonar(arguments[0], arguments[1]));
            }
        }
    }

    void numeric_variable_(MatchState const & matchState) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto args = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedArgs = evaluate_value_(args);
        if (!evaluatedArgs.first) {
            return;
        }
        variables_[varName].set(kitty_int(evaluatedArgs.second));
    }

    void viewing_variable_(MatchState const & matchState) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (find_variable_(varName)) {
            Serial.println((varName + ": " + variables_[varName].str()).c_str());
        }
    }

    void increment_by_(MatchState const & matchState) {
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
        increment_by_(matchState, increment);
    }

    void increment_by_(MatchState const & matchState, int const & increment) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!find_variable_(varName) || !check_is_value_(varName)) {
            return;
        }        
        auto value = variables_[varName].get_value();
        variables_[varName].set(kitty_int(value + increment));
    }

    void decrement_by_(MatchState const & matchState) {
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
        decrement_by_(matchState, decrement);
    }

    void decrement_by_(MatchState const & matchState, int const & decrement) {
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

    void create_if_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto ifCondition = string(matchState.capture[1].init, matchState.capture[1].len);
        auto groupActions = vector<string>();
        if (!match_brackets_(ifCondition)) {
            return;
        }
        actionGroups_.erase(groupName);
        Serial.println((string("") + "Adding actions to group " + groupName + " with condition " + ifCondition).c_str());
        auto inputLine = get_line_();
        while (inputLine != "done") {
            Serial.println((string("") + "Added >>>" + inputLine + "<<< to the group").c_str());
            groupActions.push_back(inputLine);
            inputLine = get_line_();
        }
        actionIfGroups_[groupName] = make_pair_(ifCondition, groupActions);
    }

    void create_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto groupActions = vector<string>();
        actionIfGroups_.erase(groupName);
        Serial.println((string("") + "Adding actions to group " + groupName + ":").c_str());
        auto inputLine = get_line_();
        while (inputLine != "done") {
            Serial.println((string("") + "Added >>>" + inputLine + "<<< to the group").c_str());
            groupActions.push_back(inputLine);
            inputLine = get_line_();
        }
        actionGroups_[groupName] = groupActions;
    }

    void run_group_(MatchState const & matchState) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (find_action_group_(groupName)) {
            for (auto const & action : actionGroups_[groupName]) {
                actionQueue_.push(make_pair_(action, true));
            }
        } else if (find_action_if_group_(groupName)) {
            if (evaluate_if_condition_(actionIfGroups_[groupName].first)) {
                for (auto const & action : actionIfGroups_[groupName].second) {
                    actionQueue_.push(make_pair_(action, true));
                }
            }
        } else {
            Serial.print(F("ERROR: "));
            Serial.print(groupName.c_str());
            Serial.println(F(" does not exist"));
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
