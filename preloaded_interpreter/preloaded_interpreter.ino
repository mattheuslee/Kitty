#include <StandardCplusplus.h>

#include <string>
#include <vector>

#include <kitty_interpreter.hpp>

using namespace std;
using namespace kitty;

kitty_interpreter interpreter;

void setup() {
    /*
     * Place your commands here, in the following format:

       vector<string> commands {
           "first command",
           "second command",
           "last command"
       };

    */
    // blink_led
    /*vector<string> commands {
        "light is led using 13",

        "create blink repeating",
        "move light to 100",
        "wait 1s",
        "move light to 0",
        "wait 1s",
        "done",

        "run blink"
    };*/
    // pulse_led
    vector<string> commands {
        "level is 0",
        "change is 5",
        "light is led using 13",

        "create reverse_down if level equals 100",
        "change is -5",
        "done",

        "create reverse_up if level equals 0",
        "change is 5",
        "done",

        "create pulse repeating",
        "move light to level",
        "level is level + change",
        "run reverse_down",
        "run reverse_up",
        "done",

        "run pulse"
    };
    // sos_led
    /*vector<string> commands {
        "light is led using 13",

        "create long_blink",
        "move light to 100",
        "wait 500ms",
        "move light to 0",
        "wait 500ms",
        "done",

        "create short_blink",
        "move light to 100",
        "wait 200ms",
        "move light to 0",
        "wait 200ms",
        "done",

        "create sos repeating",
        "run short_blink",
        "run short_blink",
        "run short_blink",
        "run long_blink",
        "run long_blink",
        "run long_blink",
        "run short_blink",
        "run short_blink",
        "run short_blink",
        "wait 1s",
        "done",

        "run sos"
    };*/


    interpreter.print_welcome_screen();
    interpreter.execute_preloaded_commands(commands);    
}

void loop() {

}
