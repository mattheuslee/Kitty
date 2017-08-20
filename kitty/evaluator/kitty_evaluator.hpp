#pragma once

#ifndef KITTY_EVALUATOR_HPP_
#define KITTY_EVALUATOR_HPP_

#include <StandardCplusplus.h>

#include <stack>
#include <string>

#include <Regexp.h>

#include <kitty/storage/kitty_storage.hpp>
#include <kitty/utility/kitty_pair.hpp>
#include <kitty/utility/kitty_utility.hpp>

namespace kitty {

using std::stack;
using std::string;

class kitty_evaluator {

public:
    typedef kitty_pair<bool, int> val_result_t;
    typedef kitty_pair<bool, bool> bool_result_t;

    static val_result_t evaluate_to_value(string expression, kitty_storage const & storage) {
        MatchState matchState;
        matchState.Target(expression.c_str());
        if (!match_brackets_(expression)) {
            return invalid_val_result_();
        }
        if (is_addition_expression_(matchState)) {
            return evaluate_addition_expression_(matchState, storage);
        } else if (is_subtraction_expression_(matchState)) {
            return evaluate_subtraction_expression_(matchState, storage);
        } else if (is_multiplication_expression_(matchState)) {
            return evaluate_multiplication_expression_(matchState, storage);
        } else if (is_division_expression_(matchState)) {
            return evaluate_division_expression_(matchState, storage);
        } else if (is_single_variable_expression_(matchState)) {
            return evaluate_single_variable_expression_(matchState, storage);            
        } else if (is_numerical_expression_(matchState)) {
            return evaluate_numerical_expression_(matchState, storage);            
        }
        print_unable_to_evaluate_(expression);
        return invalid_val_result_();
    }

    static bool_result_t evaluate_to_boolean(string const & expression, kitty_storage const & storage) {
        MatchState matchState;
        matchState.Target(expression.c_str());
        if (!match_brackets_(expression)) {
            return invalid_bool_result_();
        }
        if (is_true_literal_(matchState)) {
            return make_kitty_pair(true, true);
        } else if (is_false_literal_(matchState)) {
            return make_kitty_pair(true, false);
        } else if (is_less_than_comparison_(matchState)) {
            return evaluate_less_than_comparison_(matchState, storage);
        } else if (is_greater_than_comparison_(matchState)) {
            return evaluate_greater_than_comparison_(matchState, storage);
        } else if (is_equals_comparison_(matchState)) {
            return evaluate_equal_comparison_(matchState, storage);
        } else if (is_and_operation_(matchState)) {
            return evaluate_and_operation_(matchState, storage);
        } else if (is_or_operation_(matchState)) {
            return evaluate_or_operation_(matchState, storage);
        } else if (is_not_operation_(matchState)) {
            return evaluate_not_operation_(matchState, storage);
        }
        print_unable_to_evaluate_(expression);
        return invalid_bool_result_();
    }

private:
    static bool match_brackets_(string const & expression) {
        auto bracketStack = stack<char>();
        for (auto const & c : expression) {
            if (c == '(') {
                bracketStack.push(c);
            } else if (c == ')') {
                if (bracketStack.top() == '(') {
                    bracketStack.pop();
                }
            }
        }
        if (!bracketStack.empty()) {
            print_mismatched_brackets(expression);     
            return false;       
        }
        return true;
    }

    static void print_mismatched_brackets(string const & expression) {
        Serial.print(F("ERROR: brackets in expression "));
        Serial.print(expression.c_str());
        Serial.print(F(" are mismatched"));
    }

    static void print_unable_to_evaluate_(string const & expression) {
        Serial.print(F("ERROR: unable to evaluate expression "));
        Serial.println(expression.c_str());
    }

    static bool is_addition_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %+ ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_subtraction_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %- ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_multiplication_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %* ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_division_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) %/ ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_single_variable_expression_(MatchState & matchState) {
        return matchState.Match("^%(*([%a_]+)%)*$") > 0;
    }

    static bool is_numerical_expression_(MatchState & matchState) {
        return matchState.Match("^%(*(%-?%d+)%)*$") > 0;
    }

    static bool is_true_literal_(MatchState & matchState) {
        return matchState.Match("^%(*true%(*$") > 0;
    }

    static bool is_false_literal_(MatchState & matchState) {
        return matchState.Match("^%(*false%(*$") > 0;
    }

    static bool is_less_than_comparison_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) less than ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_greater_than_comparison_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) greater than ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_equals_comparison_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) equals ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_and_operation_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) and ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_or_operation_(MatchState & matchState) {
        return matchState.Match("^%(*([%a%d%+%-%(%)_ ]+) or ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static bool is_not_operation_(MatchState & matchState) {
        return matchState.Match("^%(*not ([%a%d%+%-%(%)_ ]+)%)*$") > 0;
    }

    static val_result_t evaluate_addition_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_val_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second + evaluatedSides.second.second);
    }

    static val_result_t evaluate_subtraction_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_val_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second - evaluatedSides.second.second);
    }

    static val_result_t evaluate_multiplication_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_val_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second * evaluatedSides.second.second);
    }

    static val_result_t evaluate_division_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_val_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second / evaluatedSides.second.second);
    }

    static val_result_t evaluate_single_variable_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return invalid_val_result_();
        }
        return make_kitty_pair(true, storage.variable(varName).get_value());
    }

    static val_result_t evaluate_numerical_expression_(MatchState const & matchState, kitty_storage const & storage) {
        return make_kitty_pair(true, kitty_utility::string_to_int(string(matchState.capture[0].init, matchState.capture[0].len)));
    }

    static bool_result_t evaluate_less_than_comparison_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_bool_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second < evaluatedSides.second.second);
    }

    static bool_result_t evaluate_greater_than_comparison_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_bool_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second > evaluatedSides.second.second);
    }

    static bool_result_t evaluate_equal_comparison_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_values_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_bool_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second == evaluatedSides.second.second);
    }

    static bool_result_t evaluate_and_operation_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_booleans_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_bool_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second && evaluatedSides.second.second);
    }

    static bool_result_t evaluate_or_operation_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_as_booleans_(matchState, storage);
        if (!verify_both_sides_valid_(evaluatedSides)) {
            return invalid_bool_result_();
        }
        return make_kitty_pair(true, evaluatedSides.first.second || evaluatedSides.second.second);
    }

    static bool_result_t evaluate_not_operation_(MatchState const & matchState, kitty_storage const & storage) {
        auto expression = string(matchState.capture[0].init, matchState.capture[0].len);
        auto evaluatedExpression = evaluate_to_boolean(expression, storage);
        if (!evaluatedExpression.first) {
            return invalid_bool_result_();
        }
        return make_kitty_pair(true, !evaluatedExpression.second);
    }

    static kitty_pair<bool_result_t, bool_result_t> evaluate_both_sides_as_booleans_(MatchState const & matchState, kitty_storage const & storage) {
        auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
        auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedLhs = evaluate_to_boolean(lhs, storage);
        auto evaluatedRhs = evaluate_to_boolean(rhs, storage);
        return make_kitty_pair(evaluatedLhs, evaluatedRhs);
    }

    static kitty_pair<val_result_t, val_result_t> evaluate_both_sides_as_values_(MatchState const & matchState, kitty_storage const & storage) {
        auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
        auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedLhs = evaluate_to_value(lhs, storage);
        auto evaluatedRhs = evaluate_to_value(rhs, storage);
        return make_kitty_pair(evaluatedLhs, evaluatedRhs);
    }

    template <class T>
    static bool verify_both_sides_valid_(kitty_pair<kitty_pair<bool, T>, kitty_pair<bool, T>> const & sides) {
        return sides.first.first && sides.second.first;
    }

    static val_result_t invalid_val_result_() {
        return make_kitty_pair(false, 0);
    }

    static bool_result_t invalid_bool_result_() {
        return make_kitty_pair(false, false);
    }

protected:

};

} // kitty

#endif // KITTY_EVALUATOR_HPP_
