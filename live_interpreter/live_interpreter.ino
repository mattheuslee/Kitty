#include <kitty.hpp>
#include <ArduinoLog.h>

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/interface.hpp>
#include <kty/interpreter.hpp>
#include <kty/sizes.hpp>

using namespace std;
using namespace kty;

Allocator<>         alloc;
StringPool<>        stringPool;
GetAllocInit<>      getAllocInit(alloc);
GetStringPoolInit<> getStringPoolInit(stringPool);

Interface<>         interface;
Interpreter<>       interpreter;

PoolString<>        command;
PoolString<>        prefix;

void setup() {
    interface.print_welcome();

    interface.begin_logging(LOG_LEVEL_WARNING);
    //interface.begin_logging(LOG_LEVEL_TRACE);
    //interface.begin_logging(LOG_LEVEL_SILENT);

    alloc.dump_addresses();
    stringPool.dump_addresses();
}

void loop() {
    prefix = interpreter.get_prompt_prefix();
    interface.print_prompt(prefix);
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    prefix = interpreter.get_prompt_prefix();
}
