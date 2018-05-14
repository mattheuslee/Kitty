#pragma once

#include <cstdio>

/*!
    @brief  Mock version of ArduinoLog.
*/
class MockArduinoLog {

public:
    /*!
        @brief  Constructor for mock arduino log.

        @param  toLogVerbose
                Whether verbose outputs should be shown. 

        @param  toLogTrace
                Whether trace outputs should be shown. 

        @param  toLogNotice
                Whether notice outputs should be shown.
        
        @param  toLogWarning
                Whether warning outputs should be shown.
        
        @param  toLogError
                Whether error outputs should be shown.

        @param  toLogFatal
                Whether fatal outputs should be shown.
    */
    MockArduinoLog(bool toLogVerbose = false, 
                   bool toLogTrace = false, 
                   bool toLogNotice = false, 
                   bool toLogWarning = false, 
                   bool toLogError = false, 
                   bool toLogFatal = false) 
        : toLogVerbose_(toLogVerbose), 
          toLogTrace_(toLogNotice), 
          toLogNotice_(toLogNotice), 
          toLogWarning_(toLogWarning), 
          toLogError_(toLogError), 
          toLogFatal_(toLogFatal) {
    }

    /*!
        @brief  Set whether to log verbose.

        @param  toLog
                Whether to log verbose.
    */
    void to_log_verbose(bool toLog) {
        toLogVerbose_ = toLog;
    }

    /*!
        @brief  Set whether to log trace.

        @param  toLog
                Whether to log trace.
    */
    void to_log_trace(bool toLog) {
        toLogTrace_ = toLog;
    }

    /*!
        @brief  Set whether to log notice.

        @param  toLog
                Whether to log notice.
    */
    void to_log_notice(bool toLog) {
        toLogNotice_ = toLog;
    }

    /*!
        @brief  Set whether to log warning.

        @param  toLog
                Whether to log warning.
    */
    void to_log_warning(bool toLog) {
        toLogWarning_ = toLog;
    }

    /*!
        @brief  Set whether to log error.

        @param  toLog
                Whether to log error.
    */
    void to_log_error(bool toLog) {
        toLogError_ = toLog;
    }

    /*!
        @brief  Set whether to log fatal.

        @param  toLog
                Whether to log fatal.
    */
    void to_log_fatal(bool toLog) {
        toLogFatal_ = toLog;
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
        @brief  Mock version of ArduinoLog's verbose logging.

        @param  str
                The main debug string

        @param  args
                Mock arguments.
    */
    template<typename... Args>
    void verbose(char const * str, Args... args) {
        if (toLogVerbose_) {
            printf("(VERBOSE): ");
            printf(str, args...);
        }
    }
 
    /*!
        @brief  Mock version of ArduinoLog's trace logging.

        @param  str
                The main debug string

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
        @brief  Mock version of ArduinoLog's notice logging.

        @param  str
                The main debug string

        @param  args
                Mock arguments.
    */
    template<typename... Args>
    void notice(char const * str, Args... args) {
        if (toLogNotice_) {
            printf("(NOTICE): ");
            printf(str, args...);
        }
    }

    /*!
        @brief  Mock version of ArduinoLog's warning logging.

        @param  str
                The main debug string

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

    /*!
        @brief  Mock version of ArduinoLog's error logging.

        @param  str
                The main debug string

        @param  args
                Mock arguments.
    */
    template<typename... Args>
    void error(char const * str, Args... args) {
        if (toLogError_) {
            printf("(ERROR): ");
            printf(str, args...);
        }
    }

    /*!
        @brief  Mock version of ArduinoLog's fatal logging.

        @param  str
                The main debug string

        @param  args
                Mock arguments.
    */
    template<typename... Args>
    void fatal(char const * str, Args... args) {
        if (toLogFatal_) {
            printf("(FATAL): ");
            printf(str, args...);
        }
    }

private:
    bool toLogVerbose_;
    bool toLogTrace_;
    bool toLogNotice_;
    bool toLogWarning_;
    bool toLogError_;
    bool toLogFatal_;

};
