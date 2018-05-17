#pragma once

#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>

namespace kty {

/*!
    @brief  Class that handles interactions between the user(programmer) 
            and the rest of the program.
*/
template <typename GetPoolFunc = decltype(get_stringpool), typename PoolString = PoolString<>>
class Interface {

public:
    /*!
        @brief  Constructor for the interface.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    explicit Interface(GetPoolFunc & getPoolFunc = get_stringpool) 
        : getPoolFunc_(&getPoolFunc) {
    }

    /*!
        @brief  Prints the welcome screen for the Kitty interpreter,
                as well as other information.
    */
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

    /*!
        @brief  Begins the logging using ArduinoLog

        @param  logLevel 
                Logging level parameter passed to ArduinoLog
    */
    void begin_logging(int logLevel) {
        Log.begin(logLevel, &Serial);
    }

    /*!
        @brief  Prints the basic prompt for the user to enter a command.
    */
    void print_prompt() {
        Serial.print(F(">>> "));        
    }

    /*!
        @brief  Prints the basic prompt for the user to enter a command,
                with a prefix string to be printed before the prompt.
        
        @param  prefix
                An additional string to be printed before the rest of the prompt.
    */
    void print_prompt(PoolString const & prefix) {
        if (prefix.strlen() > 0) {
            Serial.print(prefix.c_str());
        }
        print_prompt();
    }

    /*!
        @brief  Reads in a command string from the Serial interface until the
                newline character is read.
                Blocks until a complete command string is read.
        
        @return The command string read from the Serial interface.
    */
    PoolString get_next_command() {
        PoolString command(*getPoolFunc_);
        char str[2] = " "; // To use operator += on command_
        while (true) {
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n') { // Finished reading one complete line of input
                    break;
                }
                if (!isspace(c)) {
                    str[0] = c;
                    command += str;
                }
            }
        }
        return command;
    }

    /*!
        @brief  Echoes a command back to the user through the Serial interface.
        
        @param  command
                The command string to be echoed to the user.
    */
    void echo_command(PoolString const & command) {
        Serial.println(command.c_str());
    }

private:
    GetPoolFunc * getPoolFunc_;

};

} // namespace kty

