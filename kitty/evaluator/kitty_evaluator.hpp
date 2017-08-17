#pragma once

#ifndef KITTY_EVALUATOR_HPP_
#define KITTY_EVALUATOR_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>

#include <kitty/storage/kitty_storage.hpp>
#include <kitty/utility/kitty_utility.hpp>

namespace kitty {

using std::string;

class kitty_evaluator {

public:
    typedef kitty_pair<bool, int> val_result_t;

    static val_result_t evaluate_to_value(string expression, kitty_storage const & storage) {
        MatchState matchState;
        matchState.Target(expression.c_str());
        if (is_addition_expression_(matchState)) {
            return evaluate_addition_expression_(matchState, storage);
        } else if (is_subtraction_expression_(matchState)) {
            return evaluate_subtraction_expression_(matchState, storage);
        } else if (is_single_variable_expression_(matchState)) {
            return evaluate_single_variable_expression_(matchState, storage);            
        } else if (is_numerical_expression_(matchState)) {
            return evaluate_numerical_expression_(matchState, storage);            
        }
        return make_kitty_pair(false, 0);
    }

    static kitty_pair<bool, bool> evaluate_to_boolean(string const & expression, kitty_storage const & storage) {
        return make_kitty_pair(true, true);
    }

private:
    static bool is_addition_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %+ ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_subtraction_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %- ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_single_variable_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a_]+)%)*$") > 0;
    }

    static bool is_numerical_expression_(MatchState & matchState) {
        return matchState.Match("^%(*(%-?%d+)%)*$") > 0;
    }

    static val_result_t evaluate_addition_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!evaluatedSides.first.first || !evaluatedSides.second.first) { // Checking for invalid results
            return make_kitty_pair(false, 0);
        }
        return make_kitty_pair(true, evaluatedSides.first.second + evaluatedSides.second.second);
    }

    static val_result_t evaluate_subtraction_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!evaluatedSides.first.first || !evaluatedSides.second.first) { // Checking for invalid results
            return make_kitty_pair(false, 0);
        }
        return make_kitty_pair(true, evaluatedSides.first.second - evaluatedSides.second.second);
    }

    static val_result_t evaluate_single_variable_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return make_kitty_pair(false, 0);
        }
        return make_kitty_pair(true, storage.variable(varName).get_value());
    }

    static val_result_t evaluate_numerical_expression_(MatchState const & matchState, kitty_storage const & storage) {
        return make_kitty_pair(true, kitty_utility::string_to_int(string(matchState.capture[0].init, matchState.capture[0].len)));
    }

    static kitty_pair<val_result_t, val_result_t> evaluate_both_sides_(MatchState const & matchState, kitty_storage const & storage) {
        auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
        auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedLhs = evaluate_to_value(lhs, storage);
        auto evaluatedRhs = evaluate_to_value(rhs, storage);
        return make_kitty_pair(evaluatedLhs, evaluatedRhs);
    }

protected:

};

} // kitty

#endif // KITTY_EVALUATOR_HPP_
