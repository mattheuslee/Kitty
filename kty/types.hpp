#pragma once

/** Only include if not using Arduino */
#if !defined(ARDUINO)
#include <typeinfo>
#endif

#if defined(ARDUINO)
/** No type string representation for Arduino */
#define TYPE_STR(x) ""
#else
/** String representation of type */
#define TYPE_STR(x) typeid(x).name()
#endif
