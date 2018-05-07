#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

#include <kty/containers/deque.hpp>

namespace kty {

/*!
    @brief  Class that maintains all the strings in the program.
            The memory pool is created on the stack to avoid heap fragmentation.
            Holds enough memory to allocate N strings of at most size S.            
*/
template <int N, int S>
class StringPool {

public:
    /*!
        @brief  Constructor for the string pool
    */
    StringPool() {
        memset((void*)pool_, '\0', N * (S + 1));
        memset((void*)taken_, 0, N * sizeof(int));
        numTaken_ = 0;
        maxNumTaken_ = 0;
    }

    /*!
        @brief  Prints stats about the string pool.
    */
    void stat() const {
        Serial.print(F("StringPool: num taken = "));
        Serial.print(numTaken_);
        Serial.print(F(", max num taken = "));
        Serial.println(maxNumTaken_);
    }

    /*!
        @brief  Resets the stats about the string pool.
    */
    void reset_stat() {
        maxNumTaken_ = numTaken_;
    }

    /*!
        @brief  Prints the addresses used by the string database
    */
    void dump_addresses() const {
        Serial.print(F("StringPool: Pool addresses = "));
        Serial.print((unsigned int)pool_);
        Serial.print(F(" to "));
        Serial.println((unsigned int)(pool_ + N * (S + 1) - 1));
    }

    /*!
        @brief  Checks if an index is owned by this pool.

        @param  idx
                The index to check.

        @return True if the index is owned by this pool, false otherwise.
    */
    bool owns(int const & idx) {
        return idx >= 0 && idx < N;
    }

    /*!
        @brief  Check the number of available blocks left in the pool.

        @return The number of available blocks left in the pool.
    */
    int available() const {
        return N - numTaken_;
    }

    /*!
        @brief  Gets the next free index for a string.
    
        @return An index into the pool if there is space,
                -1 otherwise.
    */
    int allocate_idx() {
        for (int i = 0; i < N; ++i) {
            if (taken_[i] == 0) {
                ++taken_[i];
                ++numTaken_;
                if (numTaken_ > maxNumTaken_) {
                    maxNumTaken_ = numTaken_;
                    //Log.trace(F("StringPool::allocate_idx new maxNumTaken %d\n"), maxNumTaken_);
                }
                Log.trace(F("StringPool::allocate_idx Allocating index %d\n"), i);                
                return i;
            }
        }
        Log.warning(F("StringPool::allocate_idx No more string indices to allocate\n"));
        return -1;
    }

    /*!
        @brief  Returns the reference count for an index.

        @param  idx
                The index to get the reference count for.
        
        @return The reference count for the index. 
                If the index is invalid, -1 is returned.
    */
    int num_ref(int const & idx) {
        if (idx >=0 && idx < N) {
            return taken_[idx];
        }
        Log.warning(F("StringPool::num_ref: Index %d did not come from pool\n"), idx);
        return -1;
    }

    /*!
        @brief  Increases the reference count for an index.

        @param  idx
                The index to increase the reference count for.
        
        @return True if the operation was successful, false otherwise.
    */
    bool inc_ref_count(int const & idx) {
        if (idx >=0 && idx < N) {
            ++taken_[idx];
            return true;
        }
        Log.warning(F("StringPool::inc_ref_count: Index %d did not come from pool\n"), idx);
        return false;
    }

    /*!
        @brief  Decreases the reference count for an index.

        @param  idx
                The index to decrease the reference count for.
        
        @return True if the operation was successful, false otherwise.
    */
    bool dec_ref_count(int const & idx) {
        if (idx >=0 && idx < N) {
            --taken_[idx];
            return true;
        }
        Log.warning(F("StringPool::dec_ref_count: Index %d did not come from pool\n"), idx);
        return false;
    }

    /*!
        @brief  Returns a string index to the pool
    
        @param  idx
                The index to return to the pool

        @return True if the deallocation was successful, false otherwise.
    */
    bool deallocate_idx(int const & idx) {
        if (idx >= 0 && idx < N) {
            if (taken_[idx] > 0) {
                --taken_[idx];
            }
            else {
                Log.warning(F("StringPool::deallocate_idx Index %d has already been previously deallocated\n"), idx);
                return false;
            }
            if (taken_[idx] == 0) {
                --numTaken_;
                Log.trace(F("StringPool::deallocate_idx Index %d deallocated successfully\n"), idx);                
                return true;
            }
            else {
                Log.trace(F("StringPool::deallocate_idx Index %d is not the last reference\n"), idx);
                return true;
            }
        }
        Log.warning(F("StringPool::deallocate_idx Index %d did not come from pool\n"), idx);
        return false;
    }

    /*!
        @brief  Returns the maximum possible length for a string.

        @return The maximum possible string length.
    */
    int max_str_len() const {
        return S;
    }

    /*!
        @brief  Returns the string at a given index.

        @param  idx
                The database index for the string.

        @return str
                The stored string.
    */
    char * c_str(int const & idx) const {
        if (idx >= 0 && idx < N) {
            return const_cast<char*>(pool_) + (idx * (S + 1));
        }
        Log.warning(F("StringPool::c_str Index %d is invalid, index range is [0, %d]\n"), idx, N - 1);
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
        int copyStrLen = strlen(str);
        int lenToCopy = (S < copyStrLen ? S : copyStrLen) - i;
        strncpy(c_str(idx) + i, str, lenToCopy);
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
        int currLen = strlen(c_str(idx));
        int catStrLen = strlen(str);
        // Length to cat is minimum of remaining space and length of string to cat
        int lenToCat = ((S - currLen) < catStrLen ? (S - currLen) : catStrLen);
        strncpy(c_str(idx) + currLen, str, lenToCat);
        *(c_str(idx) + currLen + lenToCat) = '\0';
    }

private:
    char pool_[N * (S + 1)];
    int taken_[N];
    int numTaken_;
    int maxNumTaken_;

};

/*!
    @brief  Thin wrapper class around a poolstring_t that redirects
            API calls to the string pool, as well as introducing some
            API shortcuts.
*/
template <class Pool>
class PoolString {

public:
    /*!
        @brief  Constructor for a pool string.

        @param  pool
                A reference to the pool which this string is using to 
                store the actual values.
    */
    PoolString(Pool & pool) : pool_(&pool) {
        poolIdx_ = pool_->allocate_idx();
        *c_str() = '\0';
    }

#if 0
    /*!
        @brief  Constructor for a pool string.
                Assumes the index given has already been allocated by the pool.

        @param  pool
                A reference to the pool which this string is using to 
                store the actual values.
        
        @param  idx
                An already allocated index in the pool for this string.
    */
    PoolString(Pool & pool, int const & idx) : pool_(&pool) {
        poolIdx_ = idx;
        pool_->inc_ref_count(poolIdx_);
    }
#endif

    /*!
        @brief  Constructor for a pool string.

        @param  pool
                A reference to the pool which this string is using to 
                store the actual values.

        @param  str
                The initial string to store.
    */
    PoolString(Pool & pool, char const * str) : pool_(&pool) {
        Log.trace(F("PoolString::constructor(%s)\n"), str);
        if (::strlen(str) > pool_->max_str_len()) {
            Log.warning(F("PoolString::constructor length of str %d is above maximum of %d, will be truncated\n"), ::strlen(str), pool_->max_str_len());
        }
        poolIdx_ = pool_->allocate_idx();
        operator=(str);
    }

    /*!
        @brief  Copy constructor for a pool string.
                Only copies the contents of the string and the pool used, 
                such that both this and the other string are pointing to 
                different memory locations.

        @param  str
                A reference to the other pool string to copy contents from.
    */
    PoolString(PoolString const & str) {
        operator=(str);
    }

#if 0
    /*!
        @brief  Move constructor for a pool string.
                Takes over the pool as well as the pool index of the other string.

        @param  str
                A reference to the other pool string to move contents from.
    */
    PoolString(PoolString && str) : pool_(str.pool_) {
        poolIdx_ = str.poolIdx_;
        // Ensure that the other string doesn't deallocate ours
        str.poolIdx_ = -1;
    }
#endif

    /*!
        @brief  Copies another string to this string.

        @param  str
                The string to copy from.
    */
    void operator=(char const * str) {
        pool_->strcpy(poolIdx_, str);
    }

    /*!
        @brief  Copy assignment operator

        @param  str
                The other pool string to copy from.
    */
    void operator=(PoolString const & str) {
        Log.trace(F("PoolString::copy=\n"));        
        if (pool_ != nullptr && pool_->owns(poolIdx_)) {
            Log.trace(F("PoolString::copy= properly initialised string\n"));        
            pool_->deallocate_idx(poolIdx_);
        }
        else {
            Log.trace(F("PoolString::copy= non-properly initialised string\n"));        
        }
        pool_ = str.pool_;
        poolIdx_ = pool_->allocate_idx();
        pool_->strcpy(poolIdx_, str.c_str());
        Log.trace(F("PoolString::copy= done\n"));        
    }

#if 0
    /*!
        @brief  Move assignment operator

        @param  str
                The other pool string to move from.
    */
    void operator=(PoolString && str) {
        poolIdx_ = str.poolIdx_;
        pool_ = str.pool_;
        // Ensure that the other string doesn't deallocate ours
        str.poolIdx_ = -1;
    }
#endif

    /*!
        @brief  Destructor for a pool string.
    */
    ~PoolString() {
        // Only return if pool idx is valid
        if (poolIdx_ >= 0) {
            Log.trace(F("PoolString::destructor returning %d to the pool\n"), poolIdx_);
            pool_->deallocate_idx(poolIdx_);
        }
    }

    /*!
        @brief  Get the pool index of this string.

        @return The pool index of this string.
    */
    int pool_idx() const {
        return poolIdx_;
    }

    /*!
        @brief  Returns a string which is in "c-style". 
                This form is suitable for passing to Arduino serial print.
        
        @return A pointer to the first character in the string.
    */
    char* c_str() const {
        return pool_->c_str(poolIdx_);
    }

    /*!
        @brief  Copies another string to this string.

        @param  str
                The string to copy from.
    */
    void strcpy(char const * str) {
        pool_->strcpy(poolIdx_, str);
    }

    /*!
        @brief  Concatenates another string to the end of this string.

        @param  str
                The string to concatenate onto this string.
    */
    void strcat(char const * str) {
        pool_->strcat(poolIdx_, str);
    }

    /*!
        @brief  Compares another string to this string.

        @param  str
                The string to compare to.

        @return <0 if this string is < the other string,
                >0 if this string is > the other string,
                0 if the two strings are identical.
    */
    int strcmp(char const * str) const {
        return ::strcmp(c_str(), str);
    }

    /*!
        @brief  Gets the length of the string.

        @return The length of the string, in number of characters.
    */
    int strlen() const {
        return ::strlen(c_str());
    }

    /*!
        @brief  Compares another string to this string for equality

        @param  str
                The string to compare to.

        @return True if the two strings are the same, false otherwise.
    */
    bool operator==(char const * str) const {
        return strcmp(str) == 0;
    }

    /*!
        @brief  Compares another pool string to this string for equality

        @param  str
                The string to compare to.

        @return True if the two strings are the same, false otherwise.
    */
    bool operator==(PoolString const & str) const {
        return strcmp(str.c_str()) == 0;
    }

    /*!
        @brief  Appends another string to the end of this string.

        @param  str
                The string to append onto this string.
    */
    void operator+=(char const * str) {
        strcat(str);
    }

    /*!
        @brief  Appends another string to the end of this string.

        @param  str
                The string to append onto this string.
    */
    void operator+=(PoolString const & str) {
        strcat(str.c_str());
    }

private:
    int poolIdx_ = -1;
    Pool* pool_ = nullptr;

};

} // namespace kty
