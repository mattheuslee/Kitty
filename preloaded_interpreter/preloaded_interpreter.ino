#include <kitty.hpp>
#include <kitty/kitty_stl_impl.hpp>

#include <string>
#include <vector>

#include <kitty/kitty_interpreter.hpp>

using namespace std;
using namespace kitty;

kitty_interpreter interpreter;

void setup() {
    /*
     * Place your commands here, in the following format:

        string commands(R"(
            light is led using 13
            set light to 0
        
            create blink
            set light to 100 for 1s
            wait 1s
            done
        
            run blink forever
        )");

    */

    // blink_led
    /*string commands(R"(
        light is led using 13
        set light to 0
        
        create blink
        set light to 100 for 1s
        wait 1s
        done
        
        run blink forever
    )");*/

    // pulse_led
    string commands(R"(
        change is 2
        light is led using 13

        create reverse_direction
        change is change * -1
        done

        create pulse
        move light by change
        run reverse_direction if (light equals 0) or (light equals 100)
        done

        run pulse forever
    )");

    // sos_led
    /*string commands(R"(
        light is led using 13
        set light to 0

        create long_blink
        set light to 100 for 500ms
        wait 500ms
        done

        create short_blink
        set light to 100 for 200ms
        wait 200ms
        done

        create sos
        run short_blink 3 times
        run long_blink 3 times
        run short_blink 3 times
        wait 1s
        done

        run sos forever
    )");*/

    // servo_sweep
    /*string commands(R"(
        change is 1
        ser is servo using 9

        create reverse_down
        change is -1
        done

        create reverse_up
        change is 1
        done

        create sweep
        move ser by change
        run reverse_down if ser equals 180
        run reverse_up if ser equals 0
        done

        run sweep forever
    )");*/

    /*string commands(R"(
        create group
        true_result is "true!"
        print true_result
        done

        i is 5
        j is 5

        run group if ((i + 6) equals 10) or (j equals 5)
    )");*/

    interpreter.print_welcome_screen();
    interpreter.execute_preloaded_commands(commands);    
}

void loop() {

}
