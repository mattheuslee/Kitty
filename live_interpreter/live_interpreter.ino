#include <kitty.hpp>
#include <kty/stl_impl.hpp>
#include <string>
#include <vector>
#include <ArduinoLog.h>

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/interface.hpp>
#include <kty/interpreter.hpp>

using namespace std;
using namespace kty;

string command;
Allocator<50, sizeof(int) * 8> alloc;
StringPool<25, 40> stringPool;

Interface interface;
Interpreter<decltype(alloc), decltype(stringPool)> interpreter(alloc, stringPool);

void setup() {
    interface.print_welcome();
    interface.begin_logging(LOG_LEVEL_TRACE);
    //interface.begin_logging(LOG_LEVEL_SILENT);
    alloc.dump_addresses();
    stringPool.dump_addresses();
    interface.print_prompt();
}

void loop() {
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    interface.print_prompt(interpreter.get_prompt_prefix());
}
