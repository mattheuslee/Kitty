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
MockArduinoLog Log;

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/analyzer.hpp>
#include <kty/interpreter.hpp>

using namespace std;
using namespace kty;

Allocator<>         alloc;
StringPool<>        stringPool;
GetAllocInit<>      getAllocInit(alloc);
GetStringPoolInit<> getStringPoolInit(stringPool);

Analyzer<>          analyzer;
Interpreter<>       interpreter;

AnalysisResult      analysisResult;
string              strCommand;
PoolString<>        command;
PoolString<>        prefix;

int main(void) {
    Log.to_log_notice(true);
    Log.to_log_warning(true);
    Log.to_log_error(true);
    
    alloc.dump_addresses();
    stringPool.dump_addresses();

    while (1) {
        prefix = interpreter.get_prompt_prefix();
        cout << prefix.c_str() << ">>> ";
        getline(cin, strCommand);
        command = strCommand.c_str();
        analysisResult = analyzer.analyze(command);
        if (analysisResult != AnalysisResult::ERROR) {
            interpreter.execute(command);
        }
    }

    return 0;
}

#endif
