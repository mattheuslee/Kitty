#pragma once

/** Only include if not using Arduino */
#if !defined(ARDUINO)
#include <typeinfo>
#endif

#if defined(ARDUINO)
/** No type string representation for Arduino */
#define TYPE_STR(x) ""
#define PRINT_FUNC ""
#else
/** String representation of type */
#define TYPE_STR(x) typeid(x).name()
#define PRINT_FUNC __PRETTY_FUNCTION__
#endif
