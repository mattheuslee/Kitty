#pragma once

#include <kty/stl_impl.hpp> 

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Class that mimics having a Deque<Deque<PoolString>>.
*/
template <typename GetAllocFunc = decltype(get_alloc), typename GetPoolFunc = decltype(get_stringpool), typename StringPool = StringPool<Sizes::stringpool_size, Sizes::string_length>>
class DequeDequePoolString {

public:
    /** The type of pool string used in the deque */
    typedef PoolString<StringPool> poolstring_t;

    /*!
        @brief  Constructor for the deque.

        @param  stringPool
                The string pool to use.
        
        @param  alloc
                The allocator to use.
    */
    DequeDequePoolString(GetAllocFunc & getAllocFunc = get_alloc, GetPoolFunc & getPoolFunc = get_stringpool) 
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc), strings_(getAllocFunc), sizes_(getAllocFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
    }

    /*!
        @brief  Allocates new space at the front.

        @return True if successful, false otherwise.
    */
    bool push_front() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        bool result;
        result = strings_.push_front(poolstring_t(*getPoolFunc_));
        result = sizes_.push_front(0) && result;
        return result;
    }

    /*!
        @brief  Gets the number of deques.

        @return The number of deques.
    */
    int size() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        return strings_.size();
    }

    /*!
        @brief  Gets the number of strings in the ith deque.

        @param  i
                The deque index.
        
        @return The number of strings in the ith deque.
                Returns -1 if i is invalid.
    */
    int size(int const & i) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (i < 0 || i >= size()) {
            Log.warning(F("%s: accessing index i = %d when size is %d\n"), PRINT_FUNC, i, size());
            return -1;
        }
        return sizes_[i];
    }

    /*!
        @brief  Removes all deques.

        @return True if the clear was successful, false otherwise.
    */
    bool clear() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        bool result = true;
        for (int i = 0; i < size(); ++i) {
            result = clear(i) && result;
        }
        strings_.clear();
        sizes_.clear();
        return result;
    }

    /*!
        @brief  Removes all strings from the ith deque

        @param  i
                The deque index.
        
        @return True if the clear was successful, false otherwise.
    */
    bool clear(int const & i) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (i < 0 || i >= size()) {
            Log.warning(F("%s: accessing index i = %d when size is %d\n"), PRINT_FUNC, i, size());
            return false;
        }
        char* stringPoolIndices = strings_[i].c_str();
        int len = sizes_[i];
        for (int i = 0; i < len; ++i) {
            int stringPoolIdx = stringPoolIndices[i];
            (*getPoolFunc_)(nullptr)->deallocate_idx(stringPoolIdx);
        }
        strings_[i] = "";
        sizes_[i] = 0;
        return true;
    }

    /*!
        @brief  Gets the jth string of the ith deque.

        @param  i
                The deque index.
        
        @param  j
                The string index within the deque.
        
        @return The string.
                This string is a copy of the string stored in the deque.
                An empty sting is returned if i or j are invalid.
    */
    poolstring_t get_str(int const & i, int const & j) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        poolstring_t str(*getPoolFunc_);
        if (i < 0 || i >= size()) {
            Log.warning(F("%s: accessing index i = %d when size is %d\n"), PRINT_FUNC, i, size());
            return str;
        }
        if (j < 0 || j >= size(i)) {
            Log.warning(F("%s: accessing index j = %d when size[%d] is %d\n"), PRINT_FUNC, j, i, size(i));            
            return str;
        }
        int stringPoolIdx = (int)(strings_[i].c_str()[j]);
        str = (*getPoolFunc_)(nullptr)->c_str(stringPoolIdx);
        Log.verbose(F("%s: string returned is %s\n"), PRINT_FUNC, str.c_str());
        return str;
    }

    /*!
        @brief  Gets the string pool index of the jth string of the ith deque.

        @param  i
                The deque index.
        
        @param  j
                The string index within the deque.
        
        @return The string pool index.
                -1 is returned if i or j are invalid.
    */
    int get_str_idx(int const & i, int const & j) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (i < 0 || i >= size()) {
            Log.warning(F("%s: accessing index i = %d when size is %d\n"), PRINT_FUNC, i, size());
            return -1;
        }
        if (j < 0 || j >= size(i)) {
            Log.warning(F("%s: accessing index j = %d when size[%d] is %d\n"), PRINT_FUNC, j, i, size(i));            
            return -1;
        }
        int stringPoolIdx = (int)(strings_[i].c_str()[j]);
        Log.verbose(F("%s: idx is %d\n"), PRINT_FUNC, stringPoolIdx);
        return stringPoolIdx;
    }

    /*!
        @brief  Pushes a string to the back of the ith deque

        @param  i
                The deque index.

        @param  str
                The string to push to the back of the deque.
        
        @return True if successful, false otherwise.
    */
    bool push_back(int const & i, PoolString<StringPool> const & str) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (i < 0 || i >= size()) {
            Log.warning(F("DequeDequePoolString::push_back accessing index i = %d when size is %d\n"), i, size());
            return false;
        }
        int stringPoolIdx = (*getPoolFunc_)(nullptr)->allocate_idx();
        (*getPoolFunc_)(nullptr)->strcpy(stringPoolIdx, str.c_str());
        char str_[2] = " ";
        str_[0] = char(stringPoolIdx);
        strings_[i] += str_;
        sizes_[i] += 1;
        Log.verbose(F("%s: %s given stringPoolIdx %d\n"), PRINT_FUNC, str.c_str(), stringPoolIdx);
        return true;
    }

private:
    GetAllocFunc * getAllocFunc_ = nullptr;
    GetPoolFunc * getPoolFunc_ = nullptr;

    Deque<poolstring_t> strings_;
    Deque<int>          sizes_;
    
};

} // namespace kty
