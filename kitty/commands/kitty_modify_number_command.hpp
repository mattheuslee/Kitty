#pragma once

#ifndef KITTY_MODIFY_NUMBER_COMMAND_HPP_
#define KITTY_MODIFY_NUMBER_COMMAND_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>

#include <kitty/evaluator/kitty_evaluator.hpp>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/variables/kitty_int.hpp>

namespace kitty {

using std::string;
    
class kitty_modify_number_command {

public:
    static bool matches_increase_by(MatchState & matchState) {
        return matchState.Match("^increase ([%a_]+) by ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_increase(MatchState & matchState) {
        return matchState.Match("^increase ([%a_]+)$") > 0;
    }

    static bool matches_decrease_by(MatchState & matchState) {
        return matchState.Match("^decrease ([%a_]+) by ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool matches_decrease(MatchState & matchState) {
        return matchState.Match("^decrease ([%a_]+)$") > 0;
    }

    static void execute_increase_by(MatchState const & matchState, kitty_storage & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return;
        }
        auto expression = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedExpression = kitty_evaluator::evaluate_to_value(expression, storage);
        if (!evaluatedExpression.first) {
            return;
        }
        increase_(varName, evaluatedExpression.second, storage);
    }

    static void execute_increase(MatchState const & matchState, kitty_storage & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return;
        }
        increase_(varName, 1, storage);
    }

    static void execute_decrease_by(MatchState const & matchState, kitty_storage & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return;
        }
        auto expression = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedExpression = kitty_evaluator::evaluate_to_value(expression, storage);
        if (!evaluatedExpression.first) {
            return;
        }
        decrease_(varName, evaluatedExpression.second, storage);
    }

    static void execute_decrease(MatchState const & matchState, kitty_storage & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return;
        }
        decrease_(varName, 1, storage);
    }

private:
    static void increase_(string const & varName, int const & increment, kitty_storage & storage) {
        auto currValue = storage.variable(varName).get_value();
        storage.variable(varName).set(kitty_int(currValue + increment));
    }

    static void decrease_(string const & varName, int const & decrement, kitty_storage & storage) {
        auto currValue = storage.variable(varName).get_value();
        storage.variable(varName).set(kitty_int(currValue - decrement));
    }

protected:
};

} // kitty

#endif // KITTY_MODIFY_NUMBER_COMMAND_HPP_
