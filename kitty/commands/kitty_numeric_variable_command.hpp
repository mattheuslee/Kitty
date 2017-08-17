#pragma once

#ifndef KITTY_NUMERIC_VARIABLE_HPP_
#define KITTY_NUMERIC_VARIABLE_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>

#include <kitty/evaluator/kitty_evaluator.hpp>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/utility/kitty_pair.hpp>

namespace kitty {

using std::string;
    
class kitty_numeric_variable_command {

public:
    static bool matches(MatchState & matchState) {
        return matchState.Match("^([%a_]+) is ([%a%d%+%-%(%)_ ]+)") > 0;
    }

    static void execute(MatchState const & matchState, kitty_storage const & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto expression = string(matchState.capture[1].init, matchState.capture[1].len);

        auto evaluatedExpression = kitty_evaluator::evaluate_to_value(expression, storage);
        if (!evaluatedExpression.first) {
            return;
        }
        storage.variable(varName).set(kitty_int(evaluatedExpression.second));
    }

private:


protected:

};

} // kitty

#endif // KITTY_NUMERIC_VARIABLE_HPP_
