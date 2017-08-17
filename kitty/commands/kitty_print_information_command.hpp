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
    static bool matches(MatchState & matchState) {
        return matchState.Match("^([%a_]+)$") > 0;
    }

    static void execute(MatchState const & matchState, kitty_storage const & storage) {
        auto name = string(matchState.capture[0].init, matchState.capture[0].len);
        storage.print_info(name);
    }

private:


protected:

};

} // kitty

#endif // KITTY_PRINT_INFORMATION_HPP_
