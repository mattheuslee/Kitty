#pragma once

namespace kty {

/*!
    @brief  Class containing information about sizes.
*/
class Sizes {

public:
#if defined(ARDUINO)
    /** The number of bytes that makes up one allocator block. */
    static const int alloc_size = sizeof(int) * 4;
    /** The maximum number of characters per string. */
    static const int string_length = 32;
#else // When running on desktop console
    /** The number of bytes that makes up one allocator block. */
    static const int alloc_size = sizeof(int) * 8;
    /** The maximum number of characters per string. */
    static const int string_length = 128;
#endif

private:

};

} // namespace kty
