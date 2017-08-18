#pragma once

#ifndef KITTY_DEVICE_VARIABLE_COMMAND_HPP_
#define KITTY_DEVICE_VARIABLE_COMMAND_HPP_

#include <StandardCplusplus.h>

#include <sstream>
#include <string>

#include <Regexp.h>

#include <kitty/storage/kitty_storage.hpp>

namespace kitty {

using std::istringstream;
using std::string;

class kitty_device_variable_command {

public:
    static bool matches(MatchState & matchState) {
        return matchState.Match("^([%a_]+) is ([%a_]+) using ([%d%a_, ]+)$") > 0;
    }

    static void execute(MatchState const & matchState, kitty_storage & storage) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto deviceName = string(matchState.capture[1].init, matchState.capture[1].len);
        auto argList = string(matchState.capture[2].init, matchState.capture[2].len);
        MatchState argMatchState;
        argMatchState.Target(argList.c_str());
        auto numArgs = argMatchState.MatchCount("([%d%a_]+)");

        auto arguments = split_arg_list_(argList, numArgs, storage);

        add_device_(deviceName, varName, numArgs, arguments, storage);
    }

private:
    static vector<int> split_arg_list_(string const & argList, int const & numArgs, kitty_storage & storage) {
        auto arguments = vector<int>();
        istringstream iss(argList);
        for (auto i = 0; i < numArgs; ++i) {
            if (isdigit(iss.peek())) {
                auto arg = 0;
                iss >> arg;
                arguments.push_back(arg);
            } else {
                auto varName = string();
                auto c = iss.get();
                do {
                    varName += c;
                    c = iss.get();
                } while (c != ',' && c != ' ' && c != EOF);
                if (!storage.assert_variable_exists(varName) || !storage.assert_variable_is_value(varName)) {
                    return;
                }
                arguments.push_back(storage.variable(varName).get_value());
            }
            while (iss.peek() == ' ' || iss.peek() == ',') { // Clear the ',' and ' '
                iss.get();
            }
        }
        return arguments;
    }

    static void add_device_(string const & deviceName, string const & varName, int const & numArgs, vector<int> const & arguments, kitty_storage & storage) {
        if (deviceName == "servo") {
            if (numArgs != 1) {
                Serial.println(F("ERROR: wrong number of inputs for servo"));
            } else {
                storage.variable(varName).set(kitty_servo(arguments[0]));
            }
        } else if (deviceName == "led") {
            if (numArgs != 1) {
                Serial.println(F("ERROR: wrong number of inputs for LED"));
            } else {
                storage.variable(varName).set(kitty_led(arguments[0]));
            }
        } else if (deviceName == "sonar") {
            if (numArgs != 2) {
                Serial.println(F("ERROR: wrong number of inputs for sonar"));
            } else {
                storage.variable(varName).set(kitty_sonar(arguments[0], arguments[1]));
            }
        }
    }

protected:

};

} // kitty

#endif // KITTY_DEVICE_VARIABLE_COMMAND_HPP_
