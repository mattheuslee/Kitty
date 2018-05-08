#pragma once

#include <kty/stl_impl.hpp>
#if defined(ARDUINO)
#define intptr_t unsigned int
#else
#include <cstdint>
#endif
#include <cstring>

#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Class that performs allocation and deallocation of memory.
            The memory pool is created on the stack to avoid heap fragmentation.
            Holds enough memory to allocate N instances of B bytes.
*/
template <int N, int B>
class Allocator {

public:
    /*!
        @brief  Constructor for the allocator.
    */
    Allocator() {
        memset((void*)pool_, 0, N * B);
        memset((void*)taken_, false, N * sizeof(int));
        numTaken_ = 0;
        maxNumTaken_ = 0;
    }

    /*!
        @brief  Prints stats about the allocator.
    */
    void stat() const {
        Log.notice(F("%s: num taken = %d, max num taken = %d\n"), PRINT_FUNC, numTaken_, maxNumTaken_);
    }

    /*!
        @brief  Resets the stats about the allocator.
    */
    void reset_stat() {
        maxNumTaken_ = numTaken_;
    }

    /*!
        @brief  Prints the addresses used by the allocator
    */
    void dump_addresses() const {
        Log.notice(F("%s: Pool addresses = %d to %d\n"), PRINT_FUNC, (intptr_t)pool_, (intptr_t)(pool_ + N * B - 1));
    }

    /*!
        @brief  Checks if an address is owned by this allocator.

        @param  ptr
                The address to check.

        @return True if the address is owned by this allocator, false otherwise.
    */
    template <typename T>
    bool owns(T* ptr) const {
        char* ptr_ = (char*)ptr;
        return ptr_ - pool_ >= 0 && ptr_ - pool_ < N * B;
    }

    /*!
        @brief  Check the number of available blocks left in the pool.

        @return The number of available blocks left in the pool.
    */
    int available() const {
        return N - numTaken_;
    }

    /*!
        @brief  Allocates a single block of memory from the pool.
                Zeroes out memory before handing it out.

        @return A pointer to a block of memory.
                If no memory is available, nullptr is returned.
    */
    void* allocate() {
        if (numTaken_ == N) {
            Log.warning(F("%s: Could not allocate new block from pool\n"), PRINT_FUNC);
            return nullptr;
        }
        for (int i = 0; i < N; ++i) {
            if (taken_[i] == 0) {
                ++taken_[i];
                ++numTaken_;
                Log.trace(F("%s: Allocating %d\n"), PRINT_FUNC, i);
                if (numTaken_ > maxNumTaken_) {
                    maxNumTaken_ = numTaken_;
                    Log.trace(F("%s: new maxNumTaken %d\n"), PRINT_FUNC, maxNumTaken_);
                }
                memset((void*)(pool_ + (B * i)), 0, B);
                return (void*)(pool_ + (B * i));
            }
        }
    }

    /*!
        @brief  Returns a single block of memory to the pool.
                This does not call the destructor on the data stored at the block.
                The destructor for the particular data type stored at the memory location
                should be called before deallocating the memory.

        @param  ptr
                A pointer to a block of memory to be returned to the pool.

        @return True if the deallocation was successful, false otherwise.
    */
    template <typename T>
    bool deallocate(T* ptr) {
        char* ptr_ = (char*)ptr;
        if (!owns(ptr_)) {
            Log.warning(F("%s: %d given to deallocate did not come from pool\n"), PRINT_FUNC, (ptr_ - pool_) / B);
            return false;
        }
        int idx = (ptr_ - pool_) / B;
        if (taken_[idx] > 0) {
            --taken_[idx];
        }
        else {
            Log.warning(F("%s: %d given to deallocate has already been previously deallocated\n"), PRINT_FUNC, (ptr_ - pool_) / B);
            return false;
        }
        if (taken_[idx] == 0) {
            --numTaken_;
            Log.trace(F("%s: deallocated %d successfully\n"), PRINT_FUNC, (ptr_ - pool_) / B);
            return true;
        }
        else {
            Log.trace(F("%s: %d is not the last reference\n"), PRINT_FUNC, (ptr_ - pool_) / B);
            return true;
        }
    }

private:
    char pool_[N * B];
    int taken_[N];
    int numTaken_;
    int maxNumTaken_;

};

} // namespace kty
