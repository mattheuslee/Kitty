#pragma once

#ifndef KITTY_WAIT_COMMAND_HPP_
#define KITTY_WAIT_COMMAND_HPP_

#include <StandardCplusplus.h>

#include <string>

#include <Regexp.h>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/utility/kitty_utility.hpp>

namespace kitty {

class kitty_wait_command {

public:
    static bool matches(MatchState & matchState) {
        return matchState.Match("^wait (%d+)m?s$") > 0;
    }

    static void execute(MatchState & matchState, kitty_storage &) {
        auto duration = kitty_utility::string_to_int(string(matchState.capture[0].init, matchState.capture[0].len));
        duration *= get_time_multiplier_(matchState);
        delay(duration);
    }

private:
    static int get_time_multiplier_(MatchState const & matchState) {
        auto multiplier = 1;
        if (matchState.src_len < 2) {
            multiplier = 1;
        } else if (matchState.src[matchState.src_len - 2] != 'm') {
            multiplier = 1000;
        }
        return multiplier;
    }

protected:

};

}

#endif // KITTY_WAIT_COMMAND_HPP_
