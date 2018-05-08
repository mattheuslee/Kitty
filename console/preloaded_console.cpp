/*!
    Console version of live interpreter, in order to run commands
    manually without worry of running out of memory.
*/
#if !defined(ARDUINO)

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
MockArduinoLog Log(false,  // Log trace
                   false,  // Log notice
                   true); // Log warning

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/interpreter.hpp>
#include <kty/sizes.hpp>

using namespace std;
using namespace kty;

/*!
    Fill in preloaded commands here,
    one on each line, and each line separated
    with a comma.
*/
char COMMANDS[][Sizes::string_length] = {
"check_div IsGroup (",
"    If (num % div = 0) (",
"        can_div IsNumber(1)",
"    )",
"    div MoveBy(1)",
")",

"check_prime IsGroup (",
"    num",
"    can_div IsNumber(0)",
"    div IsNumber(2)",
"    If (div < num) (",
"        check_div RunGroup(num / 2)",
"    )",
"    If (can_div) (",
"        'Not prime'",
"    )",
"    Else (",
"        'Prime'",
"    )",
")",

"loop_nums IsGroup (",
"    check_prime RunGroup()",
"    num MoveBy(1)",
")",

"num IsNumber(1)",
"nun_times IsNumber(10)",

"loop_nums RunGroup(nun_times)",
};
    
Allocator<1000, Sizes::alloc_size>                 alloc;
StringPool<500, Sizes::string_length>              stringPool;
string                                             strCommand;
PoolString<decltype(stringPool)>                   command(stringPool);
PoolString<decltype(stringPool)>                   prefix(stringPool);

Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);

int main(void) {
    alloc.dump_addresses();
    stringPool.dump_addresses();

    int numInstructions = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
    for (int i = 0; i < numInstructions; ++i) {
        prefix = interpreter.get_prompt_prefix();
        cout << prefix.c_str() << ">>> " << COMMANDS[i] << endl;
        command = COMMANDS[i];
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
