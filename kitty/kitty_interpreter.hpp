#pragma once

#ifndef KITTY_INTERPRETER_HPP_
#define KITTY_INTERPRETER_HPP_

#include <StandardCplusplus.h>

#include <cctype>
#include <map>
#include <deque>
#include <sstream>
#include <string>
#include <vector>

#include <Regexp.h>

#include <kitty/commands/kitty_commands.hpp>
#include <kitty/storage/kitty_storage.hpp>
#include <kitty/variables/kitty_variables.hpp>

namespace kitty {

using std::istringstream;
using std::map;
using std::deque;
using std::string;
using std::vector;

class kitty_interpreter {
public:
    kitty_interpreter() {}

    void print_welcome_screen() {
        Serial.begin(115200);            
        Serial.println(F(R"(
            _                        
            \`*-.                    
             )  _`-.                 
            .  : \  .                
            : ^   '  \               
            ; *  _.   `*-._          
            `-.-'          `-.       
              ;       `       `.     
              :.       .        \    
             ,  \   ;  :   .-'   .   
            .  , `..;  ;  '      :   _.--.
 ,-.       '  .     |  , ;       ;.-'_.-'`
:       .*'  .      : :`-:     _.`-'`
`      `*-*      .*' ; .*`- +' 
 '.(bug)          `*-*  `*-*'        
        )"));
        Serial.println(F("Welcome to Kitty 0.0.1\nby Mattheus Lee, mattheus.lee@gmail.com, 2017"));
    }

    void print_prompt() {
        Serial.print(F(">>>"));        
    }

    void execute(string const & command) {
        storage_.add_command_to_back(command);
        while (storage_.has_more_commands()) {
            parse_and_execute_single_command_();
        }
    }

    void execute_preloaded_commands(vector<string> const & commands) {
        Serial.println(F("Kitty interpreter is in preloaded commands mode"));
        storage_.is_live_mode(false);
        for (auto const & command : commands) {
            storage_.add_command_to_back(command);
        }
        while (storage_.has_more_commands()) {
            parse_and_execute_single_command_();
        }
        Serial.println(F("Program done"));
    }

private:
    kitty_storage storage_;

    void parse_and_execute_single_command_() {
        auto command = storage_.pop_next_command();
        MatchState matchState;
        matchState.Target(command.c_str());
        if (kitty_device_variable_command::matches(matchState)) {
            kitty_device_variable_command::execute(matchState, storage_);
        } 
        else if (kitty_numeric_variable_command::matches(matchState)) {
            kitty_numeric_variable_command::execute(matchState, storage_);
        } 
        else if (kitty_modify_number_command::matches_increase_by(matchState)) {
            kitty_modify_number_command::execute_increase_by(matchState, storage_);
        } 
        else if (kitty_modify_number_command::matches_increase(matchState)) {
            kitty_modify_number_command::execute_increase(matchState, storage_);
        } 
        else if (kitty_modify_number_command::matches_decrease_by(matchState)) {
            kitty_modify_number_command::execute_decrease_by(matchState, storage_);
        } 
        else if (kitty_modify_number_command::matches_decrease(matchState)) {
            kitty_modify_number_command::execute_decrease(matchState, storage_);
        }
        /*// Sense from device
        else if (matchState.Match("^sense ([%a_]+) from ([%a_]+)$") > 0) {
            sense_from_device_(matchState);
            return;
        }*/
        else if (kitty_modify_device_command::matches_move_by_for(matchState)) {
            kitty_modify_device_command::execute_move_by_for(matchState, storage_);
        }
        else if (kitty_modify_device_command::matches_move_to_for(matchState)) {
            kitty_modify_device_command::execute_move_to_for(matchState, storage_);
        }
        else if (kitty_modify_device_command::matches_set_for(matchState)) {
            kitty_modify_device_command::execute_set_for(matchState, storage_);
        }
        else if (kitty_modify_device_command::matches_move_by(matchState)) {
            kitty_modify_device_command::execute_move_by(matchState, storage_);
        }
        else if (kitty_modify_device_command::matches_move_to(matchState)) {
            kitty_modify_device_command::execute_move_to(matchState, storage_);
        }
        else if (kitty_modify_device_command::matches_set(matchState)) {
            kitty_modify_device_command::execute_set(matchState, storage_);
        }
        else if (kitty_group_command::matches_create(matchState)) {
            kitty_group_command::execute_create(matchState, storage_);
        }
        else if (kitty_group_command::matches_run(matchState)) {
            kitty_group_command::execute_run(matchState, storage_);            
        }
        else if (kitty_group_command::matches_run_if(matchState)) {
            kitty_group_command::execute_run_if(matchState, storage_);            
        }
        else if (kitty_group_command::matches_run_while(matchState)) {
            kitty_group_command::execute_run_while(matchState, storage_);            
        }
        else if (kitty_group_command::matches_run_until(matchState)) {
            kitty_group_command::execute_run_until(matchState, storage_);            
        }
        else if (kitty_group_command::matches_run_forever(matchState)) {
            kitty_group_command::execute_run_forever(matchState, storage_);            
        }
        else if (kitty_group_command::matches_run_times(matchState)) {
            kitty_group_command::execute_run_times(matchState, storage_);            
        }
        else if (kitty_wait_command::matches(matchState)) {
            kitty_wait_command::execute(matchState, storage_);            
        }
        else if (kitty_print_information_command::matches(matchState)) {
            kitty_print_information_command::execute(matchState, storage_);
        }
        else {
            Serial.println(F("ERROR: unable to parse line"));
        }
    }

    /*void sense_from_device_(MatchState const & matchState) {
        auto varName = string(matchState.capture[0].init, matchState.capture[0].len);
        auto args = string(matchState.capture[1].init, matchState.capture[1].len);
        auto devName = string();
        auto type = string();
        if (args.find(" ") != string::npos) { // Type of sensing is specified
            devName = args.substr(0, args.find(" "));
            type = args.substr(args.find(" ") + 1);
        } else {
            devName = args;
        }
        if (!find_variable_(devName) || !check_sensor_(devName)) {
            return;
        }
        auto val = variables_[devName].sense(type);
        variables_[varName].set(kitty_int(val));
    }*/

protected:

};

} // kitty

#endif // KITTY_INTERPRETER_HPP_
