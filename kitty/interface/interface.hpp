#pragma once

#include <kitty/stl_impl.hpp>
#include <string>

#include <kitty/interpreter/interpreter.hpp>
#include <kitty/parser/parser.hpp>
#include <kitty/tokenizer/tokenizer.hpp>

namespace kitty {

namespace interface {

class Interface {

public:
    void print_welcome() {
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
           '  .     |  , ;       ;.-'_.-'`
        .*' `      : :`-:     _.`-'`
        `*-*      .*' ; .*`- +' 
     (bug)        `*-*  `*-*'        
        )"));
        Serial.println(F("Kitty 0.2.0 for Arduino (2017, 2018)"));
        Serial.println(F("Mattheus Lee, mattheus.lee@gmail.com"));
        Serial.println(F(""));
    }

    void print_prompt() {
        Serial.print(F(">>> "));        
    }

    void print_prompt(std::string const & prefix) {
        if (prefix.size() > 0) {
            Serial.print(prefix.c_str());
        }
        Serial.print(F(">>> "));        
    }

    std::string get_next_command() {
        std::string line;
        while (true) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n') { // Finished reading one complete line of input
                    break;
                }
                line += c;
            }
        }
        if (line[line.size() - 1] == ' ') {
            line.erase(line.size() - 1);
        }
        return line;
    }

    void echo_command(std::string const & command) {
        Serial.println(command.c_str());
    }

private:

};

} // namespace interface

} // namespace kitty

