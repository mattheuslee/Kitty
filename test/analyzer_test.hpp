#pragma once

#include <kty/containers/string.hpp>
#include <kty/analyzer.hpp>

using namespace kty;

test(analyzer_constructor)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test analyzer_constructor starting.");
    Analyzer<> analyzer;

    Test::min_verbosity = prevTestVerbosity;
}

test(analyzer_preprocess)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test analyzer_preprocess starting.");
    PoolString<> command;

    command = "    'Hello'    ;    print hello";
    assertEqual(analyzer.preprocess(command), AnalysisResult::OKAY);
    assertEqual(command.c_str(), " 'Hello' ");

    Test::min_verbosity = prevTestVerbosity;
}

test(analyzer_remove_comments)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test analyzer_remove_comments starting.");
    PoolString<> command;

    command = "answer IsNumber(42)  ; life, the universe, everything";
    assertEqual(analyzer.remove_comments(command), AnalysisResult::OKAY);
    assertEqual(command.c_str(), "answer IsNumber(42)  ");

    command = "; all comments";
    assertEqual(analyzer.remove_comments(command), AnalysisResult::OKAY);
    assertEqual(command.c_str(), "");

    Test::min_verbosity = prevTestVerbosity;
}

test(analyzer_check_bracket_matching)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test analyzer_check_bracket_matching starting.");
    PoolString<> command;

    command = "If (1) (";
    assertEqual(analyzer.check_bracket_matching(command), AnalysisResult::WARNING);

    command = ")";
    assertEqual(analyzer.check_bracket_matching(command), AnalysisResult::WARNING);
    
    command = "answer IsNumber(42)";
    assertEqual(analyzer.check_bracket_matching(command), AnalysisResult::OKAY);

    command = "Else (";
    assertEqual(analyzer.check_bracket_matching(command), AnalysisResult::WARNING);

    Test::min_verbosity = prevTestVerbosity;
}
