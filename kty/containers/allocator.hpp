#pragma once

#include <kty/sizes.hpp>
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
        Log.verbose(F("%s\n"), PRINT_FUNC);
        memset(reinterpret_cast<void *>(pool_), 0, N * B);
        memset(reinterpret_cast<void *>(refCount_), false, N * sizeof(int));
        numTaken_ = 0;
        maxNumTaken_ = 0;
    }

    /*!
        @brief  Prints stats about the allocator.
    */
    void stat() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Log.notice(F("%s: num taken = %d, max num taken = %d\n"), PRINT_FUNC, numTaken_, maxNumTaken_);
    }

    /*!
        @brief  Resets the stats about the allocator.
    */
    void reset_stat() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        maxNumTaken_ = numTaken_;
    }

    /*!
        @brief  Prints the addresses used by the allocator
    */
    void dump_addresses() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        Log.notice(F("%s: Pool addresses = %d to %d\n"), PRINT_FUNC, (intptr_t)pool_, (intptr_t)(pool_ + N * B - 1));
    }

    /*!
        @brief  Checks if an address is owned by this allocator.

        @param  addr
                The address to check.

        @return True if the address is owned by this allocator, false otherwise.
    */
    template <typename T>
    bool owns(T * addr) const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        char* addr_ = (char *)addr;
        return addr_ - pool_ >= 0 && addr_ - pool_ < N * B;
    }

    /*!
        @brief  Check the number of available blocks left in the pool.

        @return The number of available blocks left in the pool.
    */
    int available() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return N - numTaken_;
    }

    /*!
        @brief  Gets the address of the memory at an index.

        @param  idx
                The index of the memory in the pool.

        @return The address of the memory at that index.
    */
    void * get_addr(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return (void *)(pool_ + (B * idx));
    }

    /*!
        @brief  Gets the index in the pool of a memory address.

        @param  addr
                The address to look up.
        
        @return The index of the memory address in the pool.
    */
    template <typename T>
    int get_idx(T * addr) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return ((char *)addr - pool_) / B;
    }

    /*!
        @brief  Gets the reference count of a memory address.

        @param  addr
                The address to look up.
        
        @return The number of references to that address.
                If the address is invalid, -1 is returned.
    */
    template <typename T>
    int ref_count(T * addr) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        char * addr_ = (char *)addr;
        if (!owns(addr_)) {
            Log.warning(F("%s: idx %d is not valid\n"), PRINT_FUNC, get_idx(addr));
            return -1;
        }
        int idx = get_idx(addr);
        return refCount_[idx];
    }

    /*!
        @brief  Increases reference count of a memory address.

        @param  addr
                The address to look up.

        @return The new reference count of the memory address.
                If the address is invalid, -1 is returned.
    */
    template <typename T>
    int inc_ref_count(T * addr) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        char * addr_ = (char *)addr;
        if (!owns(addr_)) {
            Log.warning(F("%s: idx %d is not valid\n"), PRINT_FUNC, get_idx(addr));
            return -1;
        }
        int idx = get_idx(addr);
        ++refCount_[idx];
        return refCount_[idx];
    }

    /*!
        @brief  Decreases the reference count of a memory address.
                If this operation decreases the reference count to 0,
                it is not deallocated.
                To ensure that a decrease to 0 deallocates the address,
                call deallocate().

        @param  addr
                The address to look up.

        @return The new reference count of the memory address.
                If the address is invalid, -1 is returned.
    */
    template <typename T>
    int dec_ref_count(T * addr) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        char * addr_ = (char *)addr;
        if (!owns(addr_)) {
            Log.warning(F("%s: idx %d is not valid\n"), PRINT_FUNC, get_idx(addr));
            return -1;
        }
        int idx = get_idx(addr);
        --refCount_[idx];
        return refCount_[idx];
    }

    /*!
        @brief  Allocates a single block of memory from the pool.
                Zeroes out memory before handing it out.

        @return A pointer to a block of memory.
                If no memory is available, nullptr is returned.
    */
    void* allocate() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (numTaken_ == N) {
            Log.warning(F("%s: Could not allocate new block from pool\n"), PRINT_FUNC);
            return nullptr;
        }
        void * addr = nullptr;
        for (int i = 0; i < N; ++i) {
            if (refCount_[i] == 0) {
                ++refCount_[i];
                ++numTaken_;
                Log.verbose(F("%s: Allocating %d\n"), PRINT_FUNC, i);
                if (numTaken_ > maxNumTaken_) {
                    maxNumTaken_ = numTaken_;
                    Log.verbose(F("%s: new maxNumTaken %d\n"), PRINT_FUNC, maxNumTaken_);
                }
                addr = get_addr(i);
                memset(addr, 0, B);
                break;
            }
        }
        return addr;
    }

    /*!
        @brief  Returns a single block of memory to the pool.
                This does not call the destructor on the data stored at the block.
                The destructor for the particular data type stored at the memory location
                should be called before deallocating the memory.

        @param  addr
                A pointer to a block of memory to be returned to the pool.

        @return True if the deallocation was successful, false otherwise.
    */
    template <typename T>
    bool deallocate(T * addr) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        char * addr_ = (char *)addr;
        int idx = get_idx(addr_);
        if (!owns(addr_)) {
            Log.warning(F("%s: index %d given to deallocate did not come from pool\n"), PRINT_FUNC, idx);
            return false;
        }
        if (refCount_[idx] > 0) {
            --refCount_[idx];
        }
        else {
            Log.warning(F("%s: idx %d given to deallocate has already been previously deallocated\n"), PRINT_FUNC, idx);
            return false;
        }
        if (refCount_[idx] == 0) {
            Log.verbose(F("%s: deallocated idx %d successfully\n"), PRINT_FUNC, idx);
            --numTaken_;
            return true;
        }
        else {
            Log.verbose(F("%s: idx %d is not the last reference, not deallocating\n"), PRINT_FUNC, idx);
            return true;
        }
    }

private:
    char pool_[N * B];
    int refCount_[N];
    int numTaken_;
    int maxNumTaken_;

};

/*!
    @brief  Returns a pointer to an allocator.

    @param  ptr
            Used to set the address to return for the very first call.
            Subsequent calls will return this address.

    @return A pointer to an allocator.
*/
Allocator<Sizes::alloc_size, Sizes::alloc_block_size> * get_alloc(Allocator<Sizes::alloc_size, Sizes::alloc_block_size> * ptr = nullptr) {
    static Allocator<Sizes::alloc_size, Sizes::alloc_block_size> * alloc;
    if (ptr != nullptr) {
        alloc = ptr;
    }
    return alloc;
}

} // namespace kty
