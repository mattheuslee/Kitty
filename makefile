CC = g++
COV_CFLAGS = -fprofile-arcs -ftest-coverage -std=gnu++11 -I./src/PyConv -O0 -fno-inline -fno-inline-small-functions -fno-default-inline
NON_COV_CFLAGS = -Wall -std=gnu++11
CONSOLE_CFLAGS = -std=gnu++11 -g

KITTY_SRC_DIR=../KittyInterpreter/
KITTY_TEST_SRC_DIR=./test/
ARDUINO_UNIT_SRC_DIR=./arduinounit/src
ARDUINO_UNIT_SRC=${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitString.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnit.cpp
ARDUINO_UNIT_MOCK=${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockTime.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockWString.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockPrint.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockPrintable.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockStream.cpp

test : ./test/test.cpp
	$(CC) -isystem ${ARDUINO_UNIT_SRC_DIR} -isystem ${KITTY_SRC_DIR} -o test_exec $< ${ARDUINO_UNIT_SRC} ${ARDUINO_UNIT_MOCK} $(NON_COV_CFLAGS)
	./test_exec
	rm -f test_exec*

coverage : ./test/test.cpp
	$(CC) -isystem ${ARDUINO_UNIT_SRC_DIR} -isystem ${KITTY_SRC_DIR} -o $@ $< ${ARDUINO_UNIT_SRC} ${ARDUINO_UNIT_MOCK} $(COV_CFLAGS)
	./coverage
	rm -f coverage*

console : ./console/console.cpp
	$(CC) -isystem ${ARDUINO_UNIT_SRC_DIR} -isystem ${KITTY_SRC_DIR} -o console_exec $< ${ARDUINO_UNIT_SRC} ${ARDUINO_UNIT_MOCK} $(CONSOLE_CFLAGS)

run_console : console
	./console_exec

preloaded_console : ./console/preloaded_console.cpp
	$(CC) -isystem ${ARDUINO_UNIT_SRC_DIR} -isystem ${KITTY_SRC_DIR} -o preloaded_console_exec $< ${ARDUINO_UNIT_SRC} ${ARDUINO_UNIT_MOCK} $(CONSOLE_CFLAGS)

run_preloaded_console : preloaded_console
	./preloaded_console_exec
