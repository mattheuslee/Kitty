#pragma once

#if defined(ARDUINO)

#define TYPE_STR(x) ""
#define PRINT_FUNC ""

#define intptr_t unsigned int

#else

#include <typeinfo>
#define TYPE_STR(x) typeid(x).name()
#define PRINT_FUNC __PRETTY_FUNCTION__
#include <cstdint>

#endif
