#pragma once
#include <iostream>
using namespace std;

#include <kty/stl_impl.hpp>
#include <cstring>

#include <kty/sizes.hpp>
#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Class that maintains all the strings in the program.
            The memory pool is created on the stack to avoid heap fragmentation.
            Holds enough memory to allocate N strings of at most length S.            
*/
template <int N = Sizes::stringpool_size, int S = Sizes::string_length>
class StringPool {

public:
    /*!
        @brief  Constructor for the string pool
    */
    StringPool() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        memset((void*)pool_, '\0', N * (S + 1));
        memset((void*)refCount_, 0, N * sizeof(int));
        numTaken_ = 0;
        maxNumTaken_ = 0;
    }

    /*!
        @brief  Prints stats about the string pool.
    */
    void stat() const {
        Log.notice(F("%s: num taken = %d, max num taken = %d\n"), PRINT_FUNC, numTaken_, maxNumTaken_);
    }

    /*!
        @brief  Resets the stats about the string pool.
    */
    void reset_stat() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        maxNumTaken_ = numTaken_;
    }

    /*!
        @brief  Prints the addresses used by the string database
    */
    void dump_addresses() const {
        Log.notice(F("%s: Pool addresses = %d to %d\n"), PRINT_FUNC, (intptr_t)pool_, (intptr_t)(pool_ + N * (S + 1) - 1));
    }

    /*!
        @brief  Returns the maximum possible length for a string.

        @return The maximum possible string length.
    */
    int max_str_len() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return S;
    }

    /*!
        @brief  Checks if an index is owned by this pool.

        @param  idx
                The index to check.

        @return True if the index is owned by this pool, false otherwise.
    */
    bool owns(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return idx >= 0 && idx < N;
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
        @brief  Returns the reference count for an index.

        @param  idx
                The index to get the reference count for.
        
        @return The reference count for the index. 
                If the index is invalid, -1 is returned.
    */
    int ref_count(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx >=0 && idx < N) {
            return refCount_[idx];
        }
        Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
        return -1;
    }

    /*!
        @brief  Increases the reference count for an index.

        @param  idx
                The index to increase the reference count for.
        
        @return The new reference count of the index.
                If the address is invalid, -1 is returned.
    */
    int inc_ref_count(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx >=0 && idx < N) {
            ++refCount_[idx];
            return refCount_[idx];
        }
        Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
        return -1;
    }

    /*!
        @brief  Decreases the reference count for an index.
                If this operation decreases the reference count to 0,
                it is not deallocated.
                To ensure that a decrease to 0 deallocates the index,
                call deallocate_idx().

        @param  idx
                The index to decrease the reference count for.
        
        @return The new reference count of the index.
                If the address is invalid, -1 is returned.
    */
    int dec_ref_count(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx >=0 && idx < N) {
            --refCount_[idx];
            return refCount_[idx];
        }
        Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
        return -1;
    }

    /*!
        @brief  Gets the next free index for a string.
    
        @return An index into the pool if there is space,
                -1 otherwise.
    */
    int allocate_idx() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        for (int i = 0; i < N; ++i) {
            if (refCount_[i] == 0) {
                ++refCount_[i];
                ++numTaken_;
                Log.trace(F("%s: Allocating index %d\n"), PRINT_FUNC, i);
                memset((void*)(pool_ + (i * (S + 1))), '\0', S + 1);
                if (numTaken_ > maxNumTaken_) {
                    maxNumTaken_ = numTaken_;
                    Log.trace(F("%s: new maxNumTaken %d\n"), PRINT_FUNC, maxNumTaken_);
                }
                return i;
            }
        }
        Log.warning(F("%s: No more string indices to allocate\n"), PRINT_FUNC);
        return -1;
    }

    /*!
        @brief  Returns a string index to the pool
    
        @param  idx
                The index to return to the pool

        @return True if the deallocation was successful, false otherwise.
    */
    bool deallocate_idx(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx < 0 || idx >= N) {
            Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
            return false;
        }
        if (refCount_[idx] > 0) {
            --refCount_[idx];
        }
        else {
            Log.warning(F("%s: Index %d has already been previously deallocated\n"), PRINT_FUNC, idx);
            return false;
        }
        if (refCount_[idx] == 0) {
            --numTaken_;
            Log.trace(F("%s: Index %d deallocated successfully\n"), PRINT_FUNC, idx);                
            return true;
        }
        else {
            Log.trace(F("%s: Index %d is not the last reference\n"), PRINT_FUNC, idx);
            return true;
        }
    }

    /*!
        @brief  Returns the string at a given index.

        @param  idx
                The database index for the string.

        @return str
                The stored string.
    */
    char * c_str(int const & idx) const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx >= 0 && idx < N) {
            return const_cast<char *>(pool_) + (idx * (S + 1));
        }
        Log.warning(F("%s: Index %d is invalid, index range is [0, %d]\n"), PRINT_FUNC, idx, N - 1);
        return nullptr;
    }

    /*!
        @brief  Sets a string in the database, with an optional
                index to start from.

        @param  idx
                The database index for the string.

        @param  str
                The incoming string.
        
        @param  i
                The optional starting index within the string to
                start copying from.
                Default is index 0.
    */
    void strcpy(int const & idx, char const * str, int const & i = 0) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int copyStrLen = ::strlen(str);
        int lenToCopy = (S < copyStrLen ? S : copyStrLen) - i;
        ::strncpy(c_str(idx) + i, str, lenToCopy);
        *(c_str(idx) + i + lenToCopy) = '\0';
    }

    /*!
        @brief  Concatenates another string to the end of a string
                in the pool.

        @param  idx
                The database index for the string.

        @param  str
                The incoming string which will be concatenated onto the end.
    */
    void strcat(int const & idx, char const * str) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        int currLen = ::strlen(c_str(idx));
        int catStrLen = ::strlen(str);
        // Length to cat is minimum of remaining space and length of string to cat
        int lenToCat = ((S - currLen) < catStrLen ? (S - currLen) : catStrLen);
        Log.verbose(F("%s: length to cat %d\n"), PRINT_FUNC, lenToCat);
        strncpy(c_str(idx) + currLen, str, lenToCat);
        *(c_str(idx) + currLen + lenToCat) = '\0';
    }

private:
    char pool_[N * (S + 1)];
    int refCount_[N];
    int numTaken_;
    int maxNumTaken_;

};

/*!
    @brief  Returns a pointer to a stringpool.

    @param  ptr
            Used to set the address to return for the very first call.
            Subsequent calls will return this address.

    @return A pointer to a stringpool.
*/
StringPool<Sizes::stringpool_size, Sizes::string_length> * get_stringpool(StringPool<Sizes::stringpool_size, Sizes::string_length> * ptr) {
    static StringPool<Sizes::stringpool_size, Sizes::string_length> * stringPool;
    if (ptr != nullptr) {
        stringPool = ptr;
    }
    return stringPool;
}

/*!
    @brief  Class to perform setup of the get_stringpool function at the global scope.
*/
template <typename StringPool = StringPool<Sizes::stringpool_size, Sizes::string_length>>
class GetStringPoolInit {

public:
    /*!
        @brief  Executes the setup call to get_stringpool with the given string pool.

        @param  stringPool
                The string pool to setup get_stringpool with.
    */
    GetStringPoolInit(StringPool & stringPool) {
        get_stringpool(&stringPool);
    }

private:

};

} // namespace kty
