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

enum class ValueType {
    NUMBER,
    BOOLEAN,
    STRING,
    INVALID
};

string value_type_to_string(ValueType const & valueType) {
    switch (valueType) {
    case ValueType::NUMBER:
        return "number";
    case ValueType::BOOLEAN:
        return "boolean";
    case ValueType::STRING:
        return "string";
    case ValueType::INVALID:
    default:
        return "invalid";
    }
}

struct EvaluatorResult {
    string expression;

    int numberVal;
    bool booleanVal;
    string stringVal;

    ValueType valueType;

    EvaluatorResult() 
            : numberVal(0), booleanVal(false), valueType(ValueType::INVALID) {}
    EvaluatorResult(string const & _expression, int const & _numberVal)
            : expression(_expression), numberVal(_numberVal), booleanVal(false), valueType(ValueType::NUMBER) {}
    EvaluatorResult(string const & _expression, bool const & _booleanVal)
            : expression(_expression), numberVal(0), booleanVal(_booleanVal), valueType(ValueType::BOOLEAN) {}
    EvaluatorResult(string const & _expression, string const & _stringVal)
            : expression(_expression), numberVal(0), booleanVal(false), stringVal(_stringVal), valueType(ValueType::STRING) {}
};

class kitty_evaluator {

public:
    typedef kitty_pair<bool, int> val_result_t;
    typedef kitty_pair<bool, bool> bool_result_t;

    static EvaluatorResult evaluate(string expression, kitty_storage const & storage) {
        if (!match_brackets_(expression) || !match_quotations_(expression)) {
            return invalid_evaluator_result_();
        }
        MatchState matchState;
        matchState.Target(expression.c_str());
        if (is_addition_expression_(matchState)) {
            return evaluate_addition_expression_(matchState, storage);
        }
        else if (is_subtraction_expression_(matchState)) {
            return evaluate_subtraction_expression_(matchState, storage);
        }
        else if (is_multiplication_expression_(matchState)) {
            return evaluate_multiplication_expression_(matchState, storage);
        }
        else if (is_division_expression_(matchState)) {
            return evaluate_division_expression_(matchState, storage);
        }
        else if (is_less_than_comparison_(matchState)) {
            return evaluate_less_than_comparison_(matchState, storage);
        }
        else if (is_greater_than_comparison_(matchState)) {
            return evaluate_greater_than_comparison_(matchState, storage);
        }
        else if (is_equals_comparison_(matchState)) {
            return evaluate_equal_comparison_(matchState, storage);
        }
        else if (is_and_operation_(matchState)) {
            return evaluate_and_operation_(matchState, storage);
        }
        else if (is_or_operation_(matchState)) {
            return evaluate_or_operation_(matchState, storage);
        }
        else if (is_single_variable_expression_(matchState)) {
            return evaluate_single_variable_expression_(matchState, storage);            
        }
        else if (is_numerical_expression_(matchState)) {
            return evaluate_numerical_expression_(matchState, storage);            
        }
        else if (is_not_operation_(matchState)) {
            return evaluate_not_operation_(matchState, storage);
        }
        else if (is_true_literal_(matchState)) {
            return EvaluatorResult(expression, true);
        }
        else if (is_false_literal_(matchState)) {
            return EvaluatorResult(expression, false);
        }
        print_unable_to_evaluate_(expression);
        return invalid_evaluator_result_();
    }

private:
    static string get_expression_(MatchState const & matchState) {
        return string(matchState.src, matchState.src_len);
    }

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
            print_mismatched_tokens_(expression, "brackets");     
            return false;       
        }
        return true;
    }

    static bool match_quotations_(string const & expression) {
        auto quotationStack = stack<char>();
        for (auto const & c : expression) {
            if (c == '\'' || c == '\"') {
                if (quotationStack.top() == c) {
                    quotationStack.pop();
                } else {
                    quotationStack.push(c);
                }
            }
        }
        if (!quotationStack.empty()) {
            print_mismatched_tokens_(expression, "quotations");
            return false;
        }
        return true;
    }

    static void print_mismatched_tokens_(string const & expression, string const & token) {
        Serial.print(F("ERROR: "));
        Serial.print(token.c_str());
        Serial.print(F(" in expression "));
        Serial.print(expression.c_str());
        Serial.println(F(" are mismatched"));
    }

    static void print_mismatched_types_(string const & lhs, string const & rhs, ValueType const & lhsValueType, ValueType const & rhsValueType) {
        Serial.print(F("ERROR: mismatched types in expressions "));
        Serial.print(lhs.c_str());
        Serial.print(F("("));
        Serial.print(value_type_to_string(lhsValueType).c_str());
        Serial.print(F(") and "));
        Serial.print(rhs.c_str());
        Serial.print(F("("));
        Serial.print(value_type_to_string(rhsValueType).c_str());
        Serial.println(F(")"));
    }

    static void print_is_not_a_type_(string const & expression, ValueType const & actualValueType, ValueType const & expectedValueType) {
        Serial.print(F("ERROR: "));
        Serial.print(expression.c_str());
        Serial.print(F(" is of type "));
        Serial.print(value_type_to_string(actualValueType).c_str());
        Serial.print(F(" but expected type was "));
        Serial.println(value_type_to_string(expectedValueType).c_str());
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

    static EvaluatorResult evaluate_addition_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal + evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_subtraction_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal - evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_multiplication_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal * evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_division_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal / evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_less_than_comparison_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal < evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_greater_than_comparison_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal > evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_equal_comparison_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_numeric_arithmetic_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.numberVal == evaluatedSides.second.numberVal);
    }

    static EvaluatorResult evaluate_and_operation_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_boolean_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.booleanVal && evaluatedSides.second.booleanVal);
    }

    static EvaluatorResult evaluate_or_operation_(MatchState const & matchState, kitty_storage const & storage) {
        auto evaluatedSides = evaluate_both_sides_(matchState, storage);
        if (!verify_sides_for_boolean_expression_(evaluatedSides)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), evaluatedSides.first.booleanVal || evaluatedSides.second.booleanVal);
    }

    static EvaluatorResult evaluate_single_variable_expression_(MatchState const & matchState, kitty_storage const & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), storage.variable(varName).get_value());
    }

    static EvaluatorResult evaluate_numerical_expression_(MatchState const & matchState, kitty_storage const & storage) {
        return EvaluatorResult(get_expression_(matchState), kitty_utility::string_to_int(string(matchState.capture[0].init, matchState.capture[0].len)));
    }

    static EvaluatorResult evaluate_not_operation_(MatchState const & matchState, kitty_storage const & storage) {
        auto expression = string(matchState.capture[0].init, matchState.capture[0].len);
        auto evaluatedExpression = evaluate(expression, storage);
        if (evaluatedExpression.valueType != ValueType::BOOLEAN) {
            return invalid_evaluator_result_();
        }
        return EvaluatorResult(get_expression_(matchState), !evaluatedExpression.booleanVal);
    }

    static kitty_pair<EvaluatorResult, EvaluatorResult> evaluate_both_sides_(MatchState const & matchState, kitty_storage const & storage) {
        auto lhs = string(matchState.capture[0].init, matchState.capture[0].len);
        auto rhs = string(matchState.capture[1].init, matchState.capture[1].len);
        auto evaluatedLhs = evaluate(lhs, storage);
        auto evaluatedRhs = evaluate(rhs, storage);
        return make_kitty_pair(evaluatedLhs, evaluatedRhs);
    }

    static bool verify_sides_for_boolean_expression_(kitty_pair<EvaluatorResult, EvaluatorResult> const & sides) {
        return verify_both_sides_valid_(sides) && verify_both_sides_same_type_(sides) && verify_both_sides_correct_type_(sides, ValueType::BOOLEAN, ValueType::BOOLEAN);
    }

    static bool verify_sides_for_numeric_arithmetic_expression_(kitty_pair<EvaluatorResult, EvaluatorResult> const & sides) {
        return verify_both_sides_valid_(sides) && verify_both_sides_same_type_(sides) && verify_both_sides_correct_type_(sides, ValueType::NUMBER, ValueType::NUMBER);
    }

    static bool verify_both_sides_valid_(kitty_pair<EvaluatorResult, EvaluatorResult> const & sides) {
        return sides.first.valueType != ValueType::INVALID && sides.second.valueType != ValueType::INVALID;
    }

    static bool verify_both_sides_same_type_(kitty_pair<EvaluatorResult, EvaluatorResult> const & sides) {
        if (sides.first.valueType != sides.second.valueType) {
            print_mismatched_types_(sides.first.expression, sides.second.expression, sides.first.valueType, sides.second.valueType);            
            return false;
        }
        return true;
    }

    static bool verify_both_sides_correct_type_(kitty_pair<EvaluatorResult, EvaluatorResult> const & sides, ValueType const & lhsExpectedValueType, ValueType const & rhsExpectedValueType) {
        auto isCorrect = true;
        if (sides.first.valueType != lhsExpectedValueType) {
            print_is_not_a_type_(sides.first.expression, sides.first.valueType, lhsExpectedValueType);
            isCorrect = false;
        }
        if (sides.second.valueType != lhsExpectedValueType) {
            print_is_not_a_type_(sides.second.expression, sides.second.valueType, lhsExpectedValueType);
            isCorrect = false;
        }
        return isCorrect;
    }

    static EvaluatorResult invalid_evaluator_result_() {
        return EvaluatorResult();
    }

protected:

};

} // kitty

#endif // KITTY_EVALUATOR_HPP_
