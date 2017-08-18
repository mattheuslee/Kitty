#include <StandardCplusplus.h>

#include <string>
#include <vector>

#include <kitty.hpp>
#include <kitty/kitty_interpreter.hpp>

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
        "set light to 0",

        "create blink",
        "move light to 100 for 1s",
        "wait 1s",
        "done",

        "run blink forever"
    };*/

    // pulse_led
    vector<string> commands {
        "level is 0",
        "change is 5",
        "light is led using 13",

        "create reverse_down",
        "change is -5",
        "done",

        "create reverse_up",
        "change is 5",
        "done",

        "create pulse",
        "set light to level",
        "increase level by change",
        "run reverse_down if level equals 100",
        "run reverse_up if level equals 0",
        "done",

        "run pulse forever"
    };

    // sos_led
    /*vector<string> commands {
        "light is led using 13",
        "set light to 0",

        "create long_blink",
        "move light to 100 for 500ms",
        "wait 500ms",
        "done",

        "create short_blink",
        "move light to 100 for 200ms",
        "wait 200ms",
        "done",

        "create sos",
        "run short_blink 3 times",
        "run long_blink 3 times",
        "run short_blink 3 times",
        "wait 1s",
        "done",

        "run sos forever"
    };*/

    // servo_sweep
    /*vector<string> commands {
        "degree is 0",
        "change is 1",
        "ser is servo using 9",

        "create reverse_down",
        "change is -1",
        "done",

        "create reverse_up",
        "change is 1",
        "done",

        "create sweep",
        "move ser to degree",
        "increase degree by change",
        "run reverse_down if degree equals 180",
        "run reverse_up if degree equals 0",
        "done",

        "run sweep forever"
    };*/


    interpreter.print_welcome_screen();
    interpreter.execute_preloaded_commands(commands);    
}

void loop() {

}
