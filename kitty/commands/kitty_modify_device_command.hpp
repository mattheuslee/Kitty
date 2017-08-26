#pragma once

#ifndef KITTY_MODIFY_DEVICE_COMMAND_HPP_
#define KITTY_MODIFY_DEVICE_COMMAND_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>

#include <kitty/evaluator/kitty_evaluator.hpp>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/utility/kitty_utility.hpp>

namespace kitty {

using std::string;    

class kitty_modify_device_command {

public:
    static bool matches_move_by_for(MatchState & matchState) {
        return matchState.Match("^move ([%a_]+) by ([%a%d%+%-%(%)_ ]+) for (%d+)m?s$") > 0;
    }

    static bool matches_move_to_for(MatchState & matchState) {
        return matchState.Match("^move ([%a_]+) to ([%a%d%+%-%(%)_ ]+) for (%d+)m?s$") > 0;
    }

    static bool matches_set_for(MatchState & matchState) {
        return matchState.Match("^set ([%a_]+) as ([%a%d%+%-%(%)_ ]+) for (%d+)m?s$") > 0
            || matchState.Match("^set ([%a_]+) to ([%a%d%+%-%(%)_ ]+) for (%d+)m?s$") > 0
            || matchState.Match("^set ([%a_]+) ([%a%d%+%-%(%)_ ]+) for (%d+)m?s$") > 0;
    }

    static bool matches_move_by(MatchState & matchState) {
        return matchState.Match("^move ([%a_]+) by ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_move_to(MatchState & matchState) {
        return matchState.Match("^move ([%a_]+) to ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_set(MatchState & matchState) {
        return matchState.Match("^set ([%a_]+) as ([%a%d%+%-%(%)_ ]+)$") > 0
            || matchState.Match("^set ([%a_]+) to ([%a%d%+%-%(%)_ ]+)$") > 0
            || matchState.Match("^set ([%a_]+) ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static void execute_move_by(MatchState const & matchState, kitty_storage & storage) {
        auto devName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!check_exists_and_is_mover_(devName, storage)) {
            return;
        }
        auto evaluatedExpression = extract_evaluated_expression_(matchState, storage);
        if (!check_evaluated_expression_(evaluatedExpression)) {
            return;
        }
        move_by_(devName, evaluatedExpression.second, storage);
    }

    static void execute_move_to(MatchState const & matchState, kitty_storage & storage) {
        auto devName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!check_exists_and_is_mover_(devName, storage)) {
            return;
        }
        auto evaluatedExpression = extract_evaluated_expression_(matchState, storage);
        if (!check_evaluated_expression_(evaluatedExpression)) {
            return;
        }
        move_to_(devName, evaluatedExpression.second, storage);
    }

    static execute_set(MatchState const & matchState, kitty_storage & storage) {
        execute_move_to(matchState, storage);
    }

    static void execute_move_by_for(MatchState const & matchState, kitty_storage & storage) {
        auto devName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!check_exists_and_is_mover_(devName, storage)) {
            return;
        }
        auto evaluatedExpression = extract_evaluated_expression_(matchState, storage);
        if (!check_evaluated_expression_(evaluatedExpression)) {
            return;
        }
        auto timeExpression = string(matchState.capture[2].init, matchState.capture[2].len);
        auto duration = kitty_utility::string_to_int(timeExpression) * get_time_multiplier_(matchState);
        move_by_for_(devName, evaluatedExpression.second, duration, storage);
    }

    static void execute_move_to_for(MatchState const & matchState, kitty_storage & storage) {
        auto devName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!check_exists_and_is_mover_(devName, storage)) {
            return;
        }
        auto evaluatedExpression = extract_evaluated_expression_(matchState, storage);
        if (!check_evaluated_expression_(evaluatedExpression)) {
            return;
        }
        auto timeExpression = string(matchState.capture[2].init, matchState.capture[2].len);
        auto duration = kitty_utility::string_to_int(timeExpression);
        auto origCommand = string(matchState.src, matchState.src_len);
        if (origCommand[origCommand.size() - 2] != 'm') {
            duration *= 1000;
        }
        move_to_for_(devName, evaluatedExpression.second, duration, storage);
    }

    static void execute_set_for(MatchState const & matchState, kitty_storage & storage) {
        execute_move_to_for(matchState, storage);
    }

private:
    static bool check_exists_and_is_mover_(string const & devName, kitty_storage const & storage) {
        return storage.assert_variable_exists(devName) && storage.assert_variable_is_mover(devName);
    }

    template <class T>
    static bool check_evaluated_expression_(kitty_pair<bool, T> expression) {
        return expression.first;
    }

    static kitty_pair<bool, int> extract_evaluated_expression_(MatchState const & matchState, kitty_storage & storage) {
        auto expression = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedExpression = kitty_evaluator::evaluate(expression, storage);
        if (evaluatedExpression.valueType == ValueType::NUMBER) {
            return make_kitty_pair(true, evaluatedExpression.numberVal);
        }
        return make_kitty_pair(false, 0);
    }

    static int get_time_multiplier_(MatchState const & matchState) {
        auto multiplier = 1;
        if (matchState.src_len < 2) {
            multiplier = 1;
        } else if (matchState.src[matchState.src_len - 2] != 'm') {
            multiplier = 1000;
        }
        return multiplier;
    }

    static void move_by_(string const & devName, int const & amount, kitty_storage & storage) {
        storage.variable(devName).move_by(amount);
    }

    static void move_to_(string const & devName, int const & amount, kitty_storage & storage) {
        storage.variable(devName).move_to(amount);
    }

    static void move_by_for_(string const & devName, int const & amount, int const & duration, kitty_storage & storage) {
        storage.variable(devName).move_by_for(amount, duration);        
    }

    static void move_to_for_(string const & devName, int const & amount, int const & duration, kitty_storage & storage) {
        storage.variable(devName).move_to_for(amount, duration);        
    }

protected:

};

}

#endif // KITTY_MODIFY_DEVICE_COMMAND_HPP_
