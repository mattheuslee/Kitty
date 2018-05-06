#pragma once

#include <cstdio>

/*!
    @brief  Mock version of ArduinoLog.
*/
class MockArduinoLog {

public:
    /*!
        @brief  Constructor for mock arduino log.

        @param  toLogTrace
                Whether trace outputs should be shown.
        
        @param  toLogWarning
                Whether warning outputs should be shown.
    */
    MockArduinoLog(bool toLogTrace = false, bool toLogWarning = false) :toLogTrace_(toLogTrace), toLogWarning_(toLogWarning) {
    }

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
    void trace(char const * str, Args... args) {
        if (toLogTrace_) {
            printf("(TRACE): ");
            printf(str, args...);
        }
    }

    /*!
        @brief  Mock version of ArduinoLog's warning logging.

        @param  args
                Mock arguments.
    */
    template<typename... Args>
    void warning(char const * str, Args... args) {
        if (toLogWarning_) {
            printf("(WARNING): ");
            printf(str, args...);
        }
    }

private:
    bool toLogTrace_;
    bool toLogWarning_;

};
