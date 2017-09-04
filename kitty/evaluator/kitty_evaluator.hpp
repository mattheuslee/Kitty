#pragma once

#ifndef KITTY_EVALUATOR_HPP_
#define KITTY_EVALUATOR_HPP_

#include <kitty/kitty_stl_impl.hpp>

#include <cctype>
#include <stack>
#include <string>

#include <Regexp.h>

#include <kitty/containers/kitty_containers.hpp>
#include <kitty/storage/kitty_storage.hpp>
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
    static EvaluatorResult evaluate(string expression, kitty_storage const & storage) {
        expression = remove_outer_brackets_(expression);
        if (!match_brackets_(expression) || !match_quotations_(expression)) {
            return invalid_evaluator_result_();
        }
        auto splitExpression = split_expression_(expression);
        if (get<0>(splitExpression)) { // Successfully split
            return evaluate_split_expression_(splitExpression, storage);
        }
        // Cannot split, is only in one part
        return evaluate_single_part_expression_(expression, storage);
    }

private:
    static string get_expression_(MatchState const & matchState) {
        return string(matchState.src, matchState.src_len);
    }

    static kitty_tuple<bool, string, string, string> split_expression_(string const & expression) {
        if (expression[0] == '(') {
            return split_expression_using_bracket_matching_(expression);
        }
        return split_expression_using_regex_(expression);
    }

    static kitty_tuple<bool, string, string, string> split_expression_using_bracket_matching_(string const & expression) {
        auto result = kitty_tuple<bool, string, string, string>(true, "", "", "");
        auto bracketStack = stack<kitty_tuple<char, int>>();
        auto lhs = string();
        auto op = string();
        auto rhs = string();
        auto idx = 0;
        for ( ; idx < expression.size(); ++idx) {
            if (expression[idx] == '(') {
                bracketStack.push(kitty_tuple<char, int>(expression[idx], idx));
                if (idx != 0) {
                    lhs += expression[idx];
                }
            } else if (expression[idx] == ')') {
                if (bracketStack.size() == 1) { // Matches the first open bracket
                    break;
                } else {
                    bracketStack.pop();
                    lhs += expression[idx];
                }
            } else {
                lhs += expression[idx];
            }
        }
        get<1>(result) = lhs;
        idx += 2; // skip ')' and ' '
        while (!isspace(expression[idx])) {
            op += expression[idx++];
        }
        idx += 1; // skip ' '
        get<2>(result) = op;
        for ( ; idx < expression.size(); ++idx) {
            rhs += expression[idx];
        }
        get<3>(result) = rhs;
        return result;
    }

    static kitty_tuple<bool, string, string, string> split_expression_using_regex_(string const & expression) {
        MatchState matchState;
        matchState.Target(expression.c_str());
        auto identity = identify_split_expression_(matchState);
        if (get<0>(identity)) {
            return kitty_tuple<bool, string, string, string>(true, string(matchState.capture[0].init, matchState.capture[0].len), get<1>(identity), string(matchState.capture[1].init, matchState.capture[1].len));
        }
        return kitty_tuple<bool, string, string, string>(false, "", "", "");
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

    static string remove_outer_brackets_(string expression) {
        auto wereBracketsRemoved = true;
        while (wereBracketsRemoved) {
            wereBracketsRemoved = false;
            auto bracketStack = stack<kitty_tuple<char, int>>();
            for (int i = 0; i < expression.size(); ++i) {
                if (expression[i] == '(') {
                    bracketStack.push(kitty_tuple<char, int>(expression[i], i));
                } else if (expression[i] == ')') {
                    if (i != expression.size() - 1) {
                        bracketStack.pop();
                    } else if (get<1>(bracketStack.top()) == 0) { // Matching brackets are opening and closing bracket
                        expression = expression.substr(1, expression.size() - 2); // Remove first and last char;
                        wereBracketsRemoved = true;
                    }
                }
            }
        }
        return expression;
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

    static kitty_tuple<bool, string> identify_split_expression_(MatchState & matchState) {
        if (matchState.Match("^([%a%d%+%-%(%)_ ]+) %+ ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("+"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) %- ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("-"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) %* ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("*"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) %/ ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("/"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) equals ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("equals"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) less than ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("less than"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) greater than ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("greater than"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) and ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("and"));
        }
        else if (matchState.Match("^([%a%d%+%-%(%)_ ]+) or ([%a%d%+%-%(%)_ ]+)$") > 0) {
            return kitty_tuple<bool, string>(true, string("or"));
        }
        return kitty_tuple<bool, string>(false, string());
    }

    static bool is_single_variable_expression_(MatchState & matchState) {
        return matchState.Match("^([%a_]+)$") > 0;
    }

    static bool is_numerical_expression_(MatchState & matchState) {
        return matchState.Match("^(%-?%d+)$") > 0;
    }

    static bool is_not_operation_(MatchState & matchState) {
        return matchState.Match("^not ([%a%d%+%-%(%)_ ]+)$") > 0;
    }

    static bool is_true_literal_(MatchState & matchState) {
        return matchState.Match("^true$") > 0;
    }

    static bool is_false_literal_(MatchState & matchState) {
        return matchState.Match("^false$") > 0;
    }

    static EvaluatorResult evaluate_split_expression_(kitty_tuple<bool, string, string, string> const & splitExpression, kitty_storage const & storage) {
        auto evaluatedLhs = evaluate(get<1>(splitExpression), storage);
        auto evaluatedRhs = evaluate(get<3>(splitExpression), storage);
        if (get<2>(splitExpression) == "+" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal + evaluatedRhs.numberVal);            
        }
        else if (get<2>(splitExpression) == "-" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal - evaluatedRhs.numberVal);            
        }
        else if (get<2>(splitExpression) == "*" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal * evaluatedRhs.numberVal);            
        }
        else if (get<2>(splitExpression) == "/" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal / evaluatedRhs.numberVal);            
        }
        else if (get<2>(splitExpression) == "less than" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal < evaluatedRhs.numberVal);            
        }
        else if (get<2>(splitExpression) == "equals" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal == evaluatedRhs.numberVal);
        }
        else if (get<2>(splitExpression) == "greater than" && verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.numberVal > evaluatedRhs.numberVal);            
        }
        else if (get<2>(splitExpression) == "and" && verify_sides_for_boolean_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.booleanVal && evaluatedRhs.booleanVal);            
        }
        else if (get<2>(splitExpression) == "or" && verify_sides_for_boolean_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult>(evaluatedLhs, evaluatedRhs))) {
            return EvaluatorResult(get<1>(splitExpression) + " " + get<2>(splitExpression) + " " + get<3>(splitExpression), evaluatedLhs.booleanVal || evaluatedRhs.booleanVal);            
        }
        return invalid_evaluator_result_();
    }

    static EvaluatorResult evaluate_single_part_expression_(string expression, kitty_storage const & storage) {
        MatchState matchState;
        matchState.Target(expression.c_str());
        if (is_single_variable_expression_(matchState)) {
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
        return invalid_evaluator_result_();        
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

    static bool verify_sides_for_boolean_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult> const & sides) {
        return verify_both_sides_valid_(sides) && verify_both_sides_same_type_(sides) && verify_both_sides_correct_type_(sides, ValueType::BOOLEAN, ValueType::BOOLEAN);
    }

    static bool verify_sides_for_numeric_arithmetic_expression_(kitty_tuple<EvaluatorResult, EvaluatorResult> const & sides) {
        return verify_both_sides_valid_(sides) && verify_both_sides_same_type_(sides) && verify_both_sides_correct_type_(sides, ValueType::NUMBER, ValueType::NUMBER);
    }

    static bool verify_both_sides_valid_(kitty_tuple<EvaluatorResult, EvaluatorResult> const & sides) {
        return get<0>(sides).valueType != ValueType::INVALID && get<1>(sides).valueType != ValueType::INVALID;
    }

    static bool verify_both_sides_same_type_(kitty_tuple<EvaluatorResult, EvaluatorResult> const & sides) {
        if (get<0>(sides).valueType != get<1>(sides).valueType) {
            print_mismatched_types_(get<0>(sides).expression, get<1>(sides).expression, get<0>(sides).valueType, get<1>(sides).valueType);            
            return false;
        }
        return true;
    }

    static bool verify_both_sides_correct_type_(kitty_tuple<EvaluatorResult, EvaluatorResult> const & sides, ValueType const & lhsExpectedValueType, ValueType const & rhsExpectedValueType) {
        auto isCorrect = true;
        if (get<0>(sides).valueType != lhsExpectedValueType) {
            print_is_not_a_type_(get<0>(sides).expression, get<0>(sides).valueType, lhsExpectedValueType);
            isCorrect = false;
        }
        if (get<1>(sides).valueType != lhsExpectedValueType) {
            print_is_not_a_type_(get<1>(sides).expression, get<1>(sides).valueType, lhsExpectedValueType);
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
