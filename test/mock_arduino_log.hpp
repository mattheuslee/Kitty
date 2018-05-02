#pragma once

class MockArduinoLog {

public:
    void begin(int level, CppIOStream* serial) {}
 
    template<typename... Args>
    void trace(Args... args) {}

private:

};

MockArduinoLog Log;
