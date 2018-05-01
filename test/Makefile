KITTY_SRC_DIR=../
ARDUINO_UNIT_SRC_DIR=../arduinounit/src
ARDUINO_UNIT_SRC=${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitString.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnit.cpp

ARDUINO_UNIT_MOCK=${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockTime.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockWString.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockPrint.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockPrintable.cpp ${ARDUINO_UNIT_SRC_DIR}/ArduinoUnitUtility/ArduinoUnitMockStream.cpp

test : test.cpp
	g++ -isystem ${ARDUINO_UNIT_SRC_DIR} -isystem ${KITTY_SRC_DIR} -std=gnu++11 -o $@ $< ${ARDUINO_UNIT_SRC} ${ARDUINO_UNIT_MOCK} && ./test