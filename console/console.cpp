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
                   true,  // Log notice
                   true); // Log warning

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/interface.hpp>
#include <kty/interpreter.hpp>
#include <kty/sizes.hpp>

using namespace std;
using namespace kty;

Allocator<500, Sizes::alloc_size>                  alloc;
StringPool<500, Sizes::string_length>               stringPool;
string                                             strCommand;
PoolString<decltype(stringPool)>                   command(stringPool);
PoolString<decltype(stringPool)>                   prefix(stringPool);

Interface<decltype(stringPool)>                    interface(stringPool);
Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);

int main(void) {
    interface.print_welcome();
    alloc.dump_addresses();
    stringPool.dump_addresses();

    while (1) {
        prefix = interpreter.get_prompt_prefix();
        interface.print_prompt(prefix);
        getline(cin, strCommand);
        command = strCommand.c_str();
        interpreter.execute(command);
        alloc.stat();
        stringPool.stat();
    }

    return 0;
}

#endif
