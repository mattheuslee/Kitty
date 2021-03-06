#pragma once

namespace kty {

/*!
    @brief  Class containing information about sizes.
*/
class Sizes {

public:
#if defined(ARDUINO)
    /** The number of blocks in the allocator. */
    static const int alloc_size = 128;
    /** The number of bytes that makes up one allocator block. */
    static const int alloc_block_size = sizeof(int) * 6;
    /** The number of strings in the stringpool. */
    static const int stringpool_size = 64;
    /** The maximum number of characters per string. */    
    static const int string_length = 32;
#else // When running on desktop console
    /** The number of blocks in the allocator. */
    static const int alloc_size = 200;
    /** The number of bytes that makes up one allocator block. */
    static const int alloc_block_size = sizeof(int) * 16;
    /** The number of strings in the stringpool. */
    static const int stringpool_size = 200;
    /** The maximum number of characters per string. */
    static const int string_length = 128;
#endif

private:

};

} // namespace kty
