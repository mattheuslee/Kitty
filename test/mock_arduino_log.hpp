#pragma once

/*!
    @brief  Mock version of ArduinoLog.
*/
class MockArduinoLog {

public:
    /*!
        @brief  Mock version of ArduinoLog's begin.

        @param  level
                Mock level.
        
        @param  serial
                Mock serial.
    */
    void begin(int level, CppIOStream* serial) {}
 
    /*!
        @brief  Mock version of ArduinoLog's trace logging.

        @param  args
                Mock arguments.
    */
    template<typename... Args>
    void trace(Args... args) {}

private:

};
