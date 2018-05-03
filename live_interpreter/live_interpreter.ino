#include <kitty.hpp>
#include <kty/stl_impl.hpp>
#include <string>
#include <vector>
#include <ArduinoLog.h>

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/interface.hpp>
#include <kty/interpreter.hpp>
#include <kty/parser.hpp>
#include <kty/tokenizer.hpp>

using namespace std;
using namespace kty;

string command;

Allocator<Deque<int>::Node> dequeIntAlloc = Deque<int>::create_allocator(100);
Interface interface;
Interpreter interpreter(dequeIntAlloc);

void setup() {
    interface.print_welcome();
    interface.begin_logging(LOG_LEVEL_TRACE);
    interface.print_prompt();
    Serial.println(sizeof(int) * 1000);
}

void loop() {
    command = interface.get_next_command();
    interface.echo_command(command);
    interpreter.execute(command);
    interface.print_prompt(interpreter.get_prompt_prefix());
}
