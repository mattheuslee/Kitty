#pragma once

#ifndef KITTY_GROUP_COMMAND_HPP_
#define KITTY_GROUP_COMMAND_HPP_

#include <StandardCplusplus.h>

#include <string>
#include <vector>

#include <Regexp.h>

#include <kitty/evaluator/kitty_evaluator.hpp>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/utility/kitty_utility.hpp>

namespace kitty {

using std::string;
using std::vector;

class kitty_group_command {

public:
    static bool matches_create(MatchState & matchState) {
        return matchState.Match("^create ([%a_]+)$") > 0;
    }

    static bool matches_run(MatchState & matchState) {
        return matchState.Match("^run ([%a_]+)$") > 0;
    }

    static bool matches_run_if(MatchState & matchState) {
        return matchState.Match("^run ([%a_]+) if ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_run_while(MatchState & matchState) {
        return matchState.Match("^run ([%a_]+) while ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_run_until(MatchState & matchState) {
        return matchState.Match("^run ([%a_]+) until ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_run_forever(MatchState & matchState) {
        return matchState.Match("^run ([%a_]+) forever$") > 0;
    }

    static bool matches_run_times(MatchState & matchState) {
        return matchState.Match("^run ([%a_]+) (%d+) times$") > 0;        
    }

    static void execute_create(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto groupCommands = get_group_commands_(groupName, storage);
        storage.command_group(groupName) = groupCommands;
    }

    static void execute_run(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_command_group_exists(groupName)) {
            return;
        }
        add_command_group_to_deque_(groupName, storage);
    }

    static void execute_run_if(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_command_group_exists(groupName)) {
            return;
        }
        auto condition = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedCondition = kitty_evaluator::evaluate(condition, storage);
        if (evaluatedCondition.valueType == ValueType::BOOLEAN && evaluatedCondition.booleanVal) {
            add_command_group_to_deque_(groupName, storage);            
        }
    }

    static void execute_run_while(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_command_group_exists(groupName)) {
            return;
        }
        auto condition = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedCondition = kitty_evaluator::evaluate(condition, storage);
        if (evaluatedCondition.valueType == ValueType::BOOLEAN && evaluatedCondition.booleanVal) {
            storage.add_command_to_front("run " + groupName + " while " + condition);
            add_command_group_to_deque_(groupName, storage);
        }
    }

    static void execute_run_until(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_command_group_exists(groupName)) {
            return;
        }
        auto condition = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedCondition = kitty_evaluator::evaluate(condition, storage);
        if (evaluatedCondition.valueType == ValueType::BOOLEAN && !evaluatedCondition.booleanVal) {
            storage.add_command_to_front("run " + groupName + " until " + condition);
            add_command_group_to_deque_(groupName, storage);
        }
    }

    static void execute_run_forever(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_command_group_exists(groupName)) {
            return;
        }
        storage.add_command_to_front("run " + groupName + " forever");
        add_command_group_to_deque_(groupName, storage);
    }

    static void execute_run_times(MatchState const & matchState, kitty_storage & storage) {
        auto groupName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_command_group_exists(groupName)) {
            return;
        }
        auto numTimes = kitty_utility::string_to_int(string(matchState.capture[1].init, matchState.capture[1].len));
        for (auto i = 0; i < numTimes; ++i) {
            storage.add_command_to_front("run " + groupName);
        }
    }

private:
    static void add_command_group_to_deque_(string const & groupName, kitty_storage & storage) {
        auto groupCommands = storage.command_group(groupName);
        for (auto it = groupCommands.rbegin(); it != groupCommands.rend(); ++it) {
            storage.add_command_to_front(*it);
        }
    }

    static vector<string> get_group_commands_(string const & groupName, kitty_storage & storage) {
        auto groupCommands = vector<string>();
        if (storage.is_live_mode()) {
            Serial.print(groupName.c_str());
            Serial.print(F(">>>"));
            auto inputLine = kitty_utility::get_line();
            while (inputLine != "done") {
                groupCommands.push_back(inputLine);
                Serial.print(inputLine.c_str());
                Serial.println(F(" added to the group"));
                Serial.print(groupName.c_str());
                Serial.print(F(">>>"));
                inputLine = kitty_utility::get_line();
            }
            Serial.println(F("group created"));
        } else {
            while(storage.peek_next_command() != "done" && storage.peek_next_command() != "") {
                groupCommands.push_back(storage.get_next_command());
            }
            storage.pop_next_command();
        }
        return groupCommands;
    }

protected:

};

}

#endif // KITTY_GROUP_COMMAND_HPP_
