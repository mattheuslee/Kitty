#pragma once

#ifndef KITTY_PRINT_INFORMATION_HPP_
#define KITTY_PRINT_INFORMATION_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>

#include <kitty/storage/kitty_storage.hpp>

namespace kitty {

using std::string;
    
class kitty_print_information_command {

public:
    static bool matches_information(MatchState & matchState) {
        return matchState.Match("^([%a_]+)$") > 0;
    }

    static bool matches_print_variable(MatchState & matchState) {
        return matchState.Match("^print ([%a_]+)$") > 0;
    }

    static bool matches_print_string(MatchState & matchState) {
        return matchState.Match("^print \"(.+)\"$") > 0;        
    }

    static void execute_information(MatchState const & matchState, kitty_storage const & storage) {
        auto name = string(matchState.capture[0].init, matchState.capture[0].len);
        storage.print_info(name);
    }

    static bool execute_print_variable(MatchState & matchState, kitty_storage const & storage) {
        auto name = string(matchState.capture[0].init, matchState.capture[0].len);
        storage.print(name);
    }

    static bool execute_print_string(MatchState & matchState, kitty_storage const & storage) {
        Serial.println(string(matchState.capture[0].init, matchState.capture[0].len).c_str());
    }

private:


protected:

};

} // kitty

#endif // KITTY_PRINT_INFORMATION_HPP_
