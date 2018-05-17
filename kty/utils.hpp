#pragma once

#if defined(ARDUINO)
/*!
    Utility functions implemented here to remove reliance on the STL for arduino.
*/

/*!
    @brief  Swaps two items.

    @param  first
            The first item to swap.

    @param  second
            The second item to swap.
*/
template <typename T>
void swap(T & first, T & second) {
    T temp(first);
    first = second;
    second = temp;
}

#else

#include <utility>

using std::swap;

#endif
