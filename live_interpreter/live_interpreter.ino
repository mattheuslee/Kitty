#include <kitty.hpp>
#include <ArduinoLog.h>

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/interface.hpp>
#include <kty/interpreter.hpp>
#include <kty/sizes.hpp>

using namespace std;
using namespace kty;

Allocator<200, Sizes::alloc_size>                  alloc;
StringPool<75, Sizes::string_length>               stringPool;
PoolString<decltype(stringPool)>                   command(stringPool);
PoolString<decltype(stringPool)>                   prefix(stringPool);

Interface<decltype(stringPool)>                    interface(stringPool);
Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);

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
