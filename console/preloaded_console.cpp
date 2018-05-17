/*!
    Console version of live interpreter, in order to run commands
    manually without worry of running out of memory.
*/
#if !defined(ARDUINO)

/*!
    Fill in commands here.
*/
char COMMANDS[] = R"(
check_div IsGroup (
    If (num % div = 0) (
        can_div IsNumber(1)
    )
    div MoveBy(1)
)

check_prime IsGroup (
    can_div IsNumber(0)
    div IsNumber(2)
    If (div < num) (
        check_div RunGroup(num / 2)
    )
    If (can_div) (
        Print(num, ' is not prime')
    )
    Else (
        Print(num, ' is prime')
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

#include <iostream>
#include <string>

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "ArduinoUnit.h"
#include "ArduinoUnitMock.h"

CppIOStream Serial;

#include <kitty.hpp>
#include <test/mock_arduino.hpp>
#include <test/mock_arduino_log.hpp>
MockArduinoLog Log;

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/interpreter.hpp>

using namespace std;
using namespace kty;
    
Allocator<>         alloc;
StringPool<>        stringPool;
GetAllocInit<>      getAllocInit(alloc);
GetStringPoolInit<> getStringPoolInit(stringPool);

Interpreter<>       interpreter;

string              strCommand;
PoolString<>        command;
PoolString<>        prefix;

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
int get_next_command(char const * buffer, int const & startIdx, PoolString<decltype(stringPool)> & command) {
    command = "";
    char str_[2] = " "; // To use operator += on command
    int i = startIdx;
    for (i = startIdx; ; ++i) {
        char c = *(buffer + i);
        if (c == '\n') { // Finished reading one line
            ++i;
            break;
        }
        str_[0] = c;
        command += str_;
    }
    return i;
}

int main(void) {
    alloc.dump_addresses();
    stringPool.dump_addresses();

    int startIdx = 0;
    while (COMMANDS[startIdx] != '\0') {
        prefix = interpreter.get_prompt_prefix();
        startIdx = get_next_command(COMMANDS, startIdx, command);
        cout << prefix.c_str() << ">>> " << command.c_str() << endl;
        interpreter.execute(command);
    }

    while (1) {
        prefix = interpreter.get_prompt_prefix();
        cout << prefix.c_str() << ">>> ";
        getline(cin, strCommand);
        command = strCommand.c_str();
        interpreter.execute(command);
    }

    return 0;
}

#endif
