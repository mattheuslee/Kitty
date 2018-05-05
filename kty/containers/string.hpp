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
        memset((void*)taken_, false, N);
        numTaken_ = 0;
        maxNumTaken_ = 0;
    }

    /*!
        @brief  Prints stats about the string pool.
    */
    void stat() {
        Serial.print(F("StringPool: num taken = "));
        Serial.print(numTaken_);
        Serial.print(F(", max num taken = "));
        Serial.println(maxNumTaken_);
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
        @brief  Gets the next free index for a string.
    
        @return An index into the pool if there is space,
                -1 otherwise.
    */
    int allocate_idx() {
        for (int i = 0; i < N; ++i) {
            if (!taken_[i]) {
                taken_[i] = true;
                ++numTaken_;
                if (numTaken_ > maxNumTaken_) {
                    Log.trace(F("StringPool: new max num taken %d\n"), maxNumTaken_);
                    maxNumTaken_ = numTaken_;
                }
                return i;
            }
        }
        Log.warning(F("StringPool: No more string indices to allocate\n"));
        return -1;
    }

    /*!
        @brief  Returns a string index to the pool
    
        @param  idx
                The index to return to the pool

        @return True if the deallocation was successful, false otherwise.
    */
    bool deallocate_idx(int const & idx) {
        if (idx >= 0 && idx < N) {
            taken_[idx] = false;
            --numTaken_;
            return true;
        }
        Log.warning(F("StringPool: Index given to deallocate did not come from pool\n"));
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
        if (idx < N) {
            return const_cast<char*>(pool_) + (idx * (S + 1));
        }
        Log.warning(F("StringPool: Attempt to get c_str at index %d, max index possible is %d\n"), idx, N - 1);
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
    bool taken_[N];
    int numTaken_;
    int maxNumTaken_;

};

/** A poolstring_t is essentially an index into the string pool */
typedef int poolstring_t;

/*!
    @brief A specialised version of Deque that handles pool strings
*/
template <typename Alloc, typename Pool>
class StringDeque : public Deque<poolstring_t, Alloc> {

public:
    /*!
        @brief  Constructor for the string deque.

        @param  allocator
                The allocator for the deque nodes.
        
        @param  pool
                The pool allocator for the strings.
    */
    StringDeque(Alloc & allocator, Pool & pool) : Deque<poolstring_t, Alloc>(allocator), pool_(pool) {
    }

    /*!
        @brief  Pops string from the front of the deque.
                If there is no string to pop, does nothing.
    */
    void pop_front() {
        if (Deque<poolstring_t, Alloc>::is_empty()) {
            return;
        }
        pool_.deallocate_idx(Deque<poolstring_t, Alloc>::head_->next->value);
        Deque<poolstring_t, Alloc>::pop_front();
    }

    /*!
        @brief  Pops string from the front of the deque.
                If there is no string to pop, does nothing.
    */
    virtual void pop_back() {
        if (Deque<poolstring_t, Alloc>::is_empty()) {
            return;
        }
        pool_.deallocate_idx(Deque<poolstring_t, Alloc>::head_->prev->value);
        Deque<poolstring_t, Alloc>::pop_back();
    }

private:
    Pool & pool_;

};

/*!
    @brief  Class that interacts with the string pool to produce
            string-like behaviour.
            This class should only be used in simplescopes with a 
            fixed opening and closing, since it does not provide 
            adequate copy/move facilities.
*/
template <class Pool>
class PoolString {

public:
    /*!
        @brief  Constructor for a pool string.

        @param  pool
                A reference to the pool which this string is using to store the actual values.
    */
    PoolString(Pool & pool) : pool_(pool) {
        poolIdx_ = pool_.allocate_idx();
    }

    ~PoolString() {
        // Only return if pool idx is valid
        if (poolIdx_ >= 0) {
            Log.trace(F("Returning %d to the pool\n"), poolIdx_);
            pool_.deallocate_idx(poolIdx_);
        }
    }

    /*!
        @brief  Returns a string which is in "c-style". 
                This form is suitable for passing to Arduino serial print.
        
        @return A pointer to the first character in the string.
    */
    char* c_str() const {
        return pool_.c_str(poolIdx_);
    }

    /*!
        @brief  Copies another string to this string.

        @param  str
                The string to copy from.
    */
    void strcpy(char const * str) {
        pool_.strcpy(poolIdx_, str);
    }

    /*!
        @brief  Concatenates another string to the end of this string.

        @param  str
                The string to concatenate onto this string.
    */
    void strcat(char const * str) {
        pool_.strcat(poolIdx_, str);
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
        return ::strcmp(pool_.c_str(poolIdx_), str);
    }

    /*!
        @brief  Gets the length of the string.

        @return The length of the string, in number of characters.
    */
    int strlen() const {
        return ::strlen(c_str());
    }

    /*!
        @brief  Copies another string to this string.

        @param  str
                The string to copy from.
    */
    void operator=(char const * str) {
        pool_.strcpy(poolIdx_, str);
    }

    /*!
        @brief  Makes this string a copy of another pool string.

        @param  str
                The other pool string to copy the string from
    */
    void operator=(PoolString const & str) {
        pool_.strcpy(poolIdx_, str.c_str());
    }

    /*!
        @brief  Compares another string to this string for equality

        @param  str
                The string to compare to.

        @return True if the two strings are the same, false otherwise.
    */
    bool operator==(char const * str) const {
        return ::strcmp(c_str(), str) == 0;
    }

    /*!
        @brief  Compares another pool string to this string for equality

        @param  str
                The string to compare to.

        @return True if the two strings are the same, false otherwise.
    */
    bool operator==(PoolString const & str) const {
        return ::strcmp(c_str(), str.c_str()) == 0;
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
    Pool& pool_;

};

} // namespace kty
