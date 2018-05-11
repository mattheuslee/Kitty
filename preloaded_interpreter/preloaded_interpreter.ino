#include <avr/pgmspace.h>

#include <kitty.hpp>
#include <kty/stl_impl.hpp>
#include <cctype>
#include <string>
#include <vector>
#include <ArduinoLog.h>

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/interface.hpp>
#include <kty/interpreter.hpp>
#include <kty/sizes.hpp>

using namespace std;
using namespace kty;

const char COMMAND_BUFFER[] PROGMEM = R"(
check_div IsGroup (
    If (num % div = 0) (
        can_div IsNumber(1)
    )
    div MoveBy(1)
)

check_prime IsGroup (
    num
    can_div IsNumber(0)
    div IsNumber(2)
    If (div < num) (
        check_div RunGroup(num / 2)
    )
    If (can_div) (
        'Not prime'
    )
    Else (
        'Prime'
    )
)

loop_nums IsGroup (
    check_prime RunGroup()
    num MoveBy(1)
)

num IsNumber(1)
num_times IsNumber(10)

loop_nums RunGroup(num_times)
)";

Allocator<200, Sizes::alloc_size>                  alloc;
StringPool<75, Sizes::string_length>               stringPool;
PoolString<decltype(stringPool)>                   command(stringPool);
PoolString<decltype(stringPool)>                   prefix(stringPool);

Interface<decltype(stringPool)>                    interface(stringPool);
Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);

/*! 
    @brief  Reads characters from the buffer until it has a full command, 
            then saves it in the variable command.
            Starts reading from the startIdx. 
    
    @param  buffer
            The character buffer to read from.

    @param  startIdx
            The first index in the buffer to start reading from.
    
    @param  command
            Where to save the full command.

    @return The index to start reading from for the next command.
*/
int get_next_command(char const * buffer, int const & startIdx, PoolString<decltype(stringPool)>  & command) {
    command = "";
    char c;
    char str_[2] = " "; // To use operator += on command
    int i = startIdx;
    for (i = startIdx; ; ++i) {
        c = pgm_read_byte_near(buffer + i);
        if (c == '\n') { // Finished reading one line
            ++i;
            break;
        }
        if (!isspace(c)) { // Add to current command
            str_[0] = c;
            command += str_;
        }
    }
    return i;
}

void setup() {
    interface.print_welcome();

    interface.begin_logging(LOG_LEVEL_WARNING);
    //interface.begin_logging(LOG_LEVEL_TRACE);
    //interface.begin_logging(LOG_LEVEL_SILENT);

    int startIdx = 0;
    while (pgm_read_byte_near(COMMAND_BUFFER + startIdx) != '\0') {
        prefix = interpreter.get_prompt_prefix();
        interface.print_prompt(prefix);
        startIdx = get_next_command(COMMAND_BUFFER, startIdx, command);
        interface.echo_command(command);
        interpreter.execute(command);
        prefix = interpreter.get_prompt_prefix();
    }
}

void loop() {
    prefix = interpreter.get_prompt_prefix();
    interface.print_prompt(prefix);
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    prefix = interpreter.get_prompt_prefix();
}