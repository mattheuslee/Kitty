#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

#include <kty/containers/deque.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/sizes.hpp>
#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Thin wrapper class around a poolstring_t that redirects
            API calls to the string pool, as well as introducing some
            API shortcuts.
*/
template <class Pool = StringPool<Sizes::stringpool_size, Sizes::string_length>, class GetPoolFunc = decltype(get_stringpool)>
class PoolString {

public:
    /*!
        @brief  Constructor for a pool string.

        @param  pool
                A reference to the pool which this string is using to 
                store the actual values.

        @param  idx
                An already allocated index in the pool for this string.
                If not provided, a new one will be allocated from the pool.
    */
    PoolString(Pool & pool, int const & idx = -1) 
        : pool_(&pool) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx == -1) {
            poolIdx_ = alloc();
            operator=("");
        }
        else {
            poolIdx_ = idx;
            pool_->inc_ref_count(poolIdx_);
        }
    }

    /*!
        @brief  Constructor for a pool string.

        @param  pool
                A reference to the pool which this string is using to 
                store the actual values.

        @param  str
                The initial string to store.
    */
    PoolString(Pool & pool, char const * str) 
        : pool_(&pool) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (::strlen(str) > pool_->max_str_len()) {
            Log.warning(F("%s: length of str %d is above maximum of %d, will be truncated\n"), PRINT_FUNC, ::strlen(str), pool_->max_str_len());
        }
        poolIdx_ = alloc();
        operator=(str);
    }

    /*!
        @brief  Constructor for a pool string.
    */
    PoolString() 
        : getPoolFunc_(&get_stringpool) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        poolIdx_ = alloc();
        operator=("");
    }

    /*!
        @brief  Constructor for a pool string.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.

        @param  idx
                An already allocated index in the pool for this string.
                If not provided, a new one will be allocated from the pool.
    */
    PoolString(GetPoolFunc & getPoolFunc, int const & idx = -1) 
        : getPoolFunc_(&getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx == -1) {
            poolIdx_ = alloc();
            operator=("");
        }
        else {
            poolIdx_ = idx;
            (*getPoolFunc_)(nullptr)->inc_ref_count(poolIdx_);
        }
    }

    /*!
        @brief  Constructor for a pool string.

        @param  idx
                An already allocated index in the pool for this string.
                If not provided, a new one will be allocated from the pool.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    PoolString(int const & idx, GetPoolFunc & getPoolFunc = get_stringpool) 
        : getPoolFunc_(&getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (idx == -1) {
            poolIdx_ = alloc();
            operator=("");
        }
        else {
            poolIdx_ = idx;
            (*getPoolFunc_)(nullptr)->inc_ref_count(poolIdx_);
        }
    }

    /*!
        @brief  Constructor for a pool string.

        @param  getPoolFunc
                A function that returns a pointer to a string pool
                when called.

        @param  str
                The initial string to store.
    */
    PoolString(GetPoolFunc & getPoolFunc, char const * str) 
        : getPoolFunc_(&getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (::strlen(str) > pool_->max_str_len()) {
            Log.warning(F("%s: length of str %d is above maximum of %d, will be truncated\n"), PRINT_FUNC, ::strlen(str), pool_->max_str_len());
        }
        poolIdx_ = alloc();
        operator=(str);
    }

    /*!
        @brief  Constructor for a pool string.

        @param  str
                The initial string to store.

        @param  getPoolFunc
                A function that returns a pointer to a string pool
                when called.
    */
    PoolString(char const * str, GetPoolFunc & getPoolFunc = get_stringpool) 
        : getPoolFunc_(&getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (::strlen(str) > pool_->max_str_len()) {
            Log.warning(F("%s: length of str %d is above maximum of %d, will be truncated\n"), PRINT_FUNC, ::strlen(str), pool_->max_str_len());
        }
        poolIdx_ = alloc();
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

    /*!
        @brief  Copies another string to this string.

        @param  str
                The string to copy from.
        
        @return A reference to this string, after the copying
                has been performed.
    */
    PoolString& operator=(char const * str) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (pool_ != nullptr) {
            Log.verbose(F("%s: pool\n"), PRINT_FUNC);
            pool_->strcpy(poolIdx_, str);
        }
        else {
            Log.verbose(F("%s: getPoolFunc\n"), PRINT_FUNC);
            (*getPoolFunc_)(nullptr)->strcpy(poolIdx_, str);
        }
        return *this;
    }

    /*!
        @brief  Copy assignment operator

        @param  str
                The other pool string to copy from.

        @return A reference to this string, after the copying
                has been performed.
    */
    PoolString& operator=(PoolString const & str) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (pool_ != nullptr && pool_->owns(poolIdx_)) {
            Log.verbose(F("%s: properly initialised pool string\n"), PRINT_FUNC);
            pool_->deallocate_idx(poolIdx_);
        }
        else if (getPoolFunc_ != nullptr && (*getPoolFunc_)(nullptr)->owns(poolIdx_)) {
            Log.verbose(F("%s: properly initialised getPoolFunc string\n"), PRINT_FUNC);
            (*getPoolFunc_)(nullptr)->deallocate_idx(poolIdx_);
        }
        pool_ = str.pool_;
        getPoolFunc_ = str.getPoolFunc_;
        poolIdx_ = alloc();
        operator=(str.c_str());
        return *this;
    }

    /*!
        @brief  Destructor for a pool string.
    */
    ~PoolString() {
        if (poolIdx_ >= 0) {
            dalloc(poolIdx_);
        }
        /*if (pool_ != nullptr && pool_->owns(poolIdx_)) {
            Log.verbose(F("%s: returning %d: \"%s\" using pool\n"), PRINT_FUNC, poolIdx_, c_str());
            pool_->deallocate_idx(poolIdx_);
        }
        else if (getPoolFunc_ != nullptr && (*getPoolFunc_)(nullptr)->owns(poolIdx_)) {
            Log.verbose(F("%s: returning %d: \"%s\" using getPoolFunc\n"), PRINT_FUNC, poolIdx_, c_str());
            (*getPoolFunc_)(nullptr)->deallocate_idx(poolIdx_);
        }*/
    }

    /*!
        @brief  Performs the correct allocation depending on whether
                a string pool object is given, or if a function to get
                a string pool object is given.

        @return The allocated index.
    */
    int alloc() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (pool_ != nullptr) {
            Log.verbose(F("%s: pool\n"), PRINT_FUNC);
            return pool_->allocate_idx();
        }
        else {
            Log.verbose(F("%s: getPoolFunc\n"), PRINT_FUNC);
            return (*getPoolFunc_)(nullptr)->allocate_idx();
        }
    }

    /*!
        @brief  Performs the correct deallocation depending on whether
                a string pool object is given, or if a function to get
                a string pool object is given.

        @param  idx
                The index to deallocate.

        @return True if the deallocation was successful, false otherwise.
    */
    bool dalloc(int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (pool_ != nullptr) {
            Log.verbose(F("%s: pool\n"), PRINT_FUNC);
            return pool_->deallocate_idx(idx);
        }
        else {
            Log.verbose(F("%s: getPoolFunc\n"), PRINT_FUNC);
            return (*getPoolFunc_)(nullptr)->deallocate_idx(idx);
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
        if (pool_ != nullptr) {
            return pool_->c_str(poolIdx_);
        }
        else {
            return (*getPoolFunc_)(nullptr)->c_str(poolIdx_);
        }
    }

    /*!
        @brief  Copies another string to this string.

        @param  str
                The string to copy from.
    */
    void strcpy(char const * str) {
        if (pool_ != nullptr) {
            pool_->strcpy(poolIdx_, str);
        }
        else {
            (*getPoolFunc_)(nullptr)->strcpy(poolIdx_, str);
        }
    }

    /*!
        @brief  Concatenates another string to the end of this string.

        @param  str
                The string to concatenate onto this string.
    */
    void strcat(char const * str) {
        if (pool_ != nullptr) {
            pool_->strcat(poolIdx_, str);
        }
        else {
            (*getPoolFunc_)(nullptr)->strcat(poolIdx_, str);
        }
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
        @brief  Appends another string to the end of this string,  
                and returns the resulting string. 
                This string is not modified.

        @param  str
                The string to append onto this string.

        @return The resulting string after appending the given string.
    */
    PoolString operator+(char const * str) const {
        PoolString result(c_str());
        result += str;
        return result;
    }

    /*!
        @brief  Appends another string to the end of this string,  
                and returns the resulting string. 
                This string is not modified.

        @param  str
                The string to append onto this string.

        @return The resulting string after appending the given string.
    */
    PoolString operator+(PoolString const & str) const {
        PoolString result(c_str());
        result = c_str();
        result += str;
        return result;
    }

    /*!
        @brief  Appends another string to the end of this string.

        @param  str
                The string to append onto this string.

        @return A reference to the string after appending the other string.
    */
    PoolString & operator+=(char const * str) {
        strcat(str);
        return *this;
    }

    /*!
        @brief  Appends another string to the end of this string.

        @param  str
                The string to append onto this string.

        @return A reference to the string after appending the other string.
    */
    PoolString & operator+=(PoolString const & str) {
        strcat(str.c_str());
        return *this;
    }

    /*!
        @brief  Gets the character stored at an index.

        @param  i
                The index.
        
        @return A reference to the character stored at that index.
                This method has undefined behaviour if i is out of bounds.
    */
    char& operator[](int const & i) {
        return c_str()[i];
    }

    /*!
        @brief  Gets the character stored at an index.

        @param  i
                The index.
        
        @return A reference to the character stored at that index.
                This method has undefined behaviour if i is out of bounds.
    */
    char const & operator[](int const & i) const {
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
    int find(char const & c, int const & start = 0) const {
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
    int find(char const * str, int const & start = 0) const {
        int len = strlen();
        int otherLen = ::strlen(str);
        char * ptr = c_str();
        for (int startIdx = start; startIdx <= len - otherLen; ++startIdx) {
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
    int count(char const & c, int const & start = 0, int end = -1) const {
        if (end == -1) {
            end = strlen();
        }
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
                The string to be inserted will start from the position index
                given.

        @param  str
                The string to insert.
        
        @param  idx
                The index to insert the string into.
    */
    void insert(char const * str, int const & idx = 0) {
        char buffer[pool_->max_str_len() + 1] = "";
        // Copy all characters from the insert idx to the temporary buffer
        ::strcpy(buffer, c_str() + idx);
        // Insert characters
        c_str()[idx] = '\0';
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
    PoolString substr_il(int const & begin = 0, int length = -1) const {
        if (length == -1) {
            length = strlen();
        }
        int maxStrLen = pool_->max_str_len();
        char buffer[maxStrLen] = "";
        int lenToCopy = length < maxStrLen ? length : maxStrLen;
        ::strncpy(buffer, c_str() + begin, lenToCopy);
        buffer[lenToCopy] = '\0';
        PoolString substring(buffer);
        return substring;
    }

    /*!
        @brief  Generates a substring of this string.
                The substring is specified by its starting
                and ending indices.

        @param  begin
                The starting index of the substring.
        
        @param  end
                One past the ending index of the substring.

        @return The generated substring.
    */
    PoolString substr_ii(int const & begin = 0, int end = -1) const {
        if (end == -1) {
            end = strlen();
        }
        PoolString substring;
        char str_[2] = " ";
        for (int i = begin; i < end; ++i) {
            str_[0] = c_str()[i];
            substring += str_;
        }
        return substring;
    }

private:
    /** The pool index for this string. */
    int poolIdx_ = -1;
    /** A pointer to the pool for this string. */
    Pool * pool_ = nullptr;
    /** A pointer to a function that returns a pointer to a pool. */
    GetPoolFunc * getPoolFunc_ = nullptr;

};

} // namespace kty
