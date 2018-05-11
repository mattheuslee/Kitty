#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>
#if defined(ARDUINO)
#define intptr_t unsigned int
#else
#include <cstdint>
#endif

#include <kty/containers/deque.hpp>
#include <kty/types.hpp>

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
        Log.notice(F("%s: num taken = %d, max num taken = %d\n"), PRINT_FUNC, numTaken_, maxNumTaken_);
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
        Log.notice(F("%s: Pool addresses = %d to %d\n"), PRINT_FUNC, (intptr_t)pool_, (intptr_t)(pool_ + N * (S + 1) - 1));
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
                Log.trace(F("%s: Allocating index %d\n"), PRINT_FUNC, i);                
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
        Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
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
        Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
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
        Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
        return false;
    }

    /*!
        @brief  Returns a string index to the pool
    
        @param  idx
                The index to return to the pool

        @return True if the deallocation was successful, false otherwise.
    */
    bool deallocate_idx(int const & idx) {
        if (idx < 0 || idx >= N) {
            Log.warning(F("%s: Index %d did not come from pool\n"), PRINT_FUNC, idx);
            return false;
        }
        if (taken_[idx] > 0) {
            --taken_[idx];
        }
        else {
            Log.warning(F("%s: Index %d has already been previously deallocated\n"), PRINT_FUNC, idx);
            return false;
        }
        if (taken_[idx] == 0) {
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
    explicit PoolString(Pool & pool) : pool_(&pool) {
        poolIdx_ = pool_->allocate_idx();
        *c_str() = '\0';
    }

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

    /*!
        @brief  Constructor for a pool string.

        @param  pool
                A reference to the pool which this string is using to 
                store the actual values.

        @param  str
                The initial string to store.
    */
    PoolString(Pool & pool, char const * str) : pool_(&pool) {
        Log.trace(F("%s: str = \n"), PRINT_FUNC, str);
        if (::strlen(str) > pool_->max_str_len()) {
            Log.warning(F("%s: length of str %d is above maximum of %d, will be truncated\n"), PRINT_FUNC, ::strlen(str), pool_->max_str_len());
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

        @return A reference to this string, after the copying
                has been performed.
    */
    PoolString& operator=(PoolString const & str) {
        Log.trace(F("%s: str = %s\n"), PRINT_FUNC, str.c_str());        
        if (pool_ != nullptr && pool_->owns(poolIdx_)) {
            Log.trace(F("%s: properly initialised string\n"), PRINT_FUNC);        
            pool_->deallocate_idx(poolIdx_);
        }
        else {
            Log.trace(F("%s: non-properly initialised string\n"), PRINT_FUNC);        
        }
        pool_ = str.pool_;
        poolIdx_ = pool_->allocate_idx();
        pool_->strcpy(poolIdx_, str.c_str());
        Log.trace(F("%s: done\n"), PRINT_FUNC);
        return *this;        
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
            Log.trace(F("%s: returning %d: %s to the pool\n"), PRINT_FUNC, poolIdx_, c_str());
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

    /*!
        @brief  Gets the character stored at an index.

        @param  i
                The index.
        
        @return A reference to the character stored at that index.
    */
    char& operator[](int const & i) {
        return c_str()[i];
    }

    /*!
        @brief  Searches for a character within this string.

        @param  c
                The character to search for.

        @param  start
                The index to start searching. Optional parameter, default is 0.

        @return The index where the character can be found, 
                or -1 if it cannot be found.
    */
    int find(char const & c, int const & start = 0) {
        int len = strlen();
        char * ptr = c_str();
        for (int i = start; i < len; ++i) {
            if (ptr[i] == c) {
                return i;
            }
        }
        return -1;
    }

    /*!
        @brief  Searches for another string within this string.

        @param  str
                The string to search for.

        @param  start
                The index to start searching. Optional parameter, default is 0.

        @return The starting index where the string can be found, 
                or -1 if it cannot be found.
    */
    int find(char const * str, int const & start = 0) {
        int len = strlen();
        int otherLen = ::strlen(str);
        char * ptr = c_str();
        for (int startIdx = start; startIdx < len - otherLen; ++startIdx) {
            bool matches = true;
            for (int i = 0; i < otherLen; ++i) {
                if (ptr[startIdx + i] != str[i]) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                return startIdx;
            }
        }
        return -1;
    }

    /*!
        @brief  Counts the number of occurences of a character in this string.

        @param  c
                The character to count.

        @param  start
                The index to start looking from. Default is 0 to start from the beginning.
        
        @param  end
                One past the last index to stop looking. Default is strlen() to stop at the end.
        
        @return The number of times the character appears.
    */
    int count(char const & c, int const & start = 0, int const & end = strlen()) {
        int counter = 0;
        char * str = c_str();
        for (int i = start; i < end; ++i) {
            if (str[i] == c) {
                ++counter;
            }
        }
        return counter;
    }

    /*!
        @brief  Inserts a string into this string at the specified location.

        @param  str
                The string to insert.
        
        @param  pos
                The position to insert the string into.
    */
    void insert(char const * str, int const & pos = 0) {
        char buffer[pool_->max_str_len() + 1];
        // Copy all characters after the insert position to the buffer
        ::strcpy(buffer, c_str() + pos);
        // Insert characters
        c_str()[pos] = '\0';
        operator+=(str);
        // Put back characters after inserted string
        operator+=(buffer);
    }

    /*!
        @brief  Generates a substring of this string.
                The substring is specified by its starting index
                and its length.

        @param  begin
                The starting index of the substring.
        
        @param  length
                The length of the substring.

        @return The generated substring.
    */
    PoolString<Pool> substr_il(int const & begin = 0, int const & length = strlen()) {
        char buffer[pool_->max_str_len()];
        ::strncpy(buffer, c_str() + begin, length);
        PoolString<Pool> substring(*pool_, buffer);
        return substring;
    }

    /*!
        @brief  Generates a substring of this string.
                The substring is specified by its indices.

        @param  begin
                The starting index of the substring.
        
        @param  end
                One past the ending index of the substring.

        @return The generated substring.
    */
    PoolString<Pool> substr_i(int const & begin = 0, int const & end = strlen()) {
        return substr_il(begin, end - begin);
    }

private:
    int poolIdx_ = -1;
    Pool* pool_ = nullptr;

};

} // namespace kty
