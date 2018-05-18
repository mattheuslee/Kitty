#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/string_utils.hpp>
#include <kty/types.hpp>

namespace kty {

/** The various results of performing analysis on commands. */
enum AnalysisResult {
    OKAY = 0,
    WARNING = 1,
    ERROR = 2,
};

/*!
    @brief  Class that performs static analysis on commands.
*/
template <typename GetAllocFunc = decltype(get_alloc), typename GetPoolFunc = decltype(get_stringpool), typename PoolString = PoolString<>>
class Analyzer {

public:
    /*!
        @brief Analyzer constructor.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    explicit Analyzer(GetAllocFunc & getAllocFunc = get_alloc, GetPoolFunc & getPoolFunc = get_stringpool) 
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
    }

    /*!
        @brief  Performs full analysis on a command.

        @param  command
                The command to analyse

        @return The result of the analysis.
    */
    AnalysisResult analyze(PoolString & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        AnalysisResult result = AnalysisResult::OKAY, tempResult;

        tempResult = preprocess(command);
        result = result > tempResult ? result : tempResult;

        tempResult = check_bracket_matching(command);
        result = result > tempResult ? result : tempResult;
    }

    /*!
        @brief  Performs preprocessing on a command.
                Preprocessing involves removing comments and reducing
                multiple consecutive whitespace to a single ' '.
        
        @param  command
                The command to preprocess.
            
        @return The result of the preprocessing.
    */
    AnalysisResult preprocess(PoolString & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        AnalysisResult result = AnalysisResult::OKAY, tempResult;
        
        tempResult = remove_comments(command);
        result = result > tempResult ? result : tempResult;
        
        remove_str_multiple_whitespace(command);

        return result;
    }

    /*!
        @brief  Removes comments from a command.
                Comments begin with the ';' character.
        
        @param  command
                The command to remove comments from.

        @return The result of removing the comments.
    */
    AnalysisResult remove_comments(PoolString & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        int commentStartIdx = command.find(';');
        if (commentStartIdx >= 0) {
            PoolString result(command.substr_ii(0, commentStartIdx));
            command = result;
        }
        return AnalysisResult::OKAY;
    }

    /*!
        @brief  Checks for matching brackets within a command.
        
        @param  command
                The command to check brackets for.

        @return The result of checking the brackets.
    */
    AnalysisResult check_bracket_matching(PoolString const & command) {
        Log.verbose(F("%s\n"), PRINT_FUNC);        
        Deque<char> bracketStack;
        int len = command.strlen();
        for (int i = 0; i < len; ++i) {
            if (command[i] == '(') {
                bracketStack.push_back(')');
            }
            else if (command[i] == ')') {
                // No matching '('
                if (bracketStack.is_empty()) {
                    Log.warning(F("%s: brackets don't match\n"), PRINT_FUNC);        
                    return AnalysisResult::WARNING;
                }
                bracketStack.pop_back();
            }
        }
        // Too many '('
        if (!bracketStack.is_empty()) {
            Log.warning(F("%s: brackets don't match\n"), PRINT_FUNC);        
            return AnalysisResult::WARNING;
        }
        return AnalysisResult::OKAY;
    }

private:
    GetAllocFunc * getAllocFunc_;
    GetPoolFunc * getPoolFunc_;

};

} // namespace kty
