#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

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
        memset((void*)pool_, '\0', N * B);
        memset((void*)taken_, false, N);
        numTaken_ = 0;
        maxNumTaken_ = 0;
    }

    /*!
        @brief  Prints stats about the allocator.
    */
    void stat() {
        Serial.print(F("Allocator: num taken = "));
        Serial.print(numTaken_);
        Serial.print(F(", max num taken = "));
        Serial.println(maxNumTaken_);
    }

    /*!
        @brief  Prints the addresses used by the allocator
    */
    void dump_addresses() {
        Serial.print(F("Allocator: Pool addresses = "));
        Serial.print((unsigned int)pool_);
        Serial.print(F(" to "));
        Serial.println((unsigned int)(pool_ + N * B - 1));
    }

    /*!
        @brief  Checks if an address is owned by this allocator.

        @param  ptr
                The address to check.

        @return True if the address is owned by this allocator, false otherwise.
    */
    template <typename T>
    bool owns(T* ptr) {
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

        @return A pointer to a block of memory.
                If no memory is available, nullptr is returned.
    */
    void* allocate() {
        for (int i = 0; i < N; ++i) {
            if (!taken_[i]) {
                taken_[i] = true;
                ++numTaken_;
                if (numTaken_ > maxNumTaken_) {
                    maxNumTaken_ = numTaken_;
                    //Log.trace(F("Allocator: new max num taken %d\n"), maxNumTaken_);
                }
                return (void*)(pool_ + (B * i));
            }
        }
        Log.warning(F("Allocator: Could not allocate new block from pool\n"));
        return nullptr;
    }

    /*!
        @brief  Returns a single block of memory to the pool.
                This does not call the destructor on the data stored at the block.

        @param  ptr
                A pointer to a block of memory to be returned to the pool.

        @return True if the deallocation was successful, false otherwise.
    */
    template <typename T>
    bool deallocate(T* ptr) {
        char* ptr_ = (char*)ptr;
        if (!owns(ptr_)) {
            Log.warning(F("Allocator: Pointer given to deallocate did not come from pool\n"));
            return false;
        }
        if (taken_[(ptr_ - pool_) / B]) {
            taken_[(ptr_ - pool_) / B] = false;
            --numTaken_;
            return true;
        }
        Log.warning(F("Allocator: Pointer given to deallocate has already been previously deallocated\n"));
        return false;
    }

private:
    char pool_[N * B];
    bool taken_[N];
    int numTaken_;
    int maxNumTaken_;

};

} // namespace kty
