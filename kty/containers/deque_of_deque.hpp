#pragma once

#include <kty/stl_impl.hpp> 

#include <kty/containers/deque.hpp>
#include <kty/containers/string.hpp>

namespace kty {

/*!
    @brief  Class that mimics having a Deque<Deque<PoolString>>.
*/
template <typename StringPool, typename Alloc>
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
    DequeDequePoolString(StringPool & stringPool, Alloc & alloc) 
            : stringPool_(&stringPool), strings_(alloc), sizes_(alloc) {
    }

    /*!
        @brief  Allocates new space at the front.
    */
    void push_front() {
        strings_.push_front(poolstring_t(*stringPool_));
        sizes_.push_front(0);
    }

    /*!
        @brief  Gets the number of deques.

        @return The number of deques.
    */
    int size() {
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
        if (i < 0 || i >= size()) {
            Log.warning(F("DequeDequePoolString::size accessing index i = %d when size is %d (undefined behaviour)\n"), i, size());
            return -1;
        }
        return sizes_[i];
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
        poolstring_t str(*stringPool_);
        Log.trace(F("DequeDequePoolString::get_str(%d, %d)\n"), i, j);
        if (i < 0 || i >= size()) {
            Log.warning(F("DequeDequePoolString::get_str accessing index i = %d when size is %d\n"), i, size());
            return str;
        }
        if (j < 0 || j >= size(i)) {
            Log.warning(F("DequeDequePoolString::get_str accessing index j = %d when size[%d] is %d\n"), j, i, size(i));            
            return str;
        }
        int stringPoolIdx = (int)(strings_[i].c_str()[j]);
        str = stringPool_->c_str(stringPoolIdx);
        Log.trace(F("DequeDequePoolString::get_str result is %s\n"), str.c_str());
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
        Log.trace(F("%s: (%d, %d)\n"), PRINT_FUNC, i, j);
        if (i < 0 || i >= size()) {
            Log.warning(F("%s: accessing index i = %d when size is %d\n"), PRINT_FUNC, i, size());
            return -1;
        }
        if (j < 0 || j >= size(i)) {
            Log.warning(F("%s: accessing index j = %d when size[%d] is %d\n"), PRINT_FUNC, j, i, size(i));            
            return -1;
        }
        int stringPoolIdx = (int)(strings_[i].c_str()[j]);
        Log.trace(F("%s: idx is %d\n"), PRINT_FUNC, stringPoolIdx);
        return stringPoolIdx;
    }

    /*!
        @brief  Removes all deques.
    */
    void clear() {
        Log.trace(F("%s\n"), PRINT_FUNC);
        for (int i = 0; i < size(); ++i) {
            clear(i);
        }
        strings_.clear();
        sizes_.clear();
    }

    /*!
        @brief  Removes all strings from the ith deque

        @param  i
                The deque index.
    */
    void clear(int const & i) {
        Log.trace(F("%s: (%d)\n"), PRINT_FUNC, i);
        if (i < 0 || i >= size()) {
            Log.warning(F("%s: accessing index i = %d when size is %d\n"), PRINT_FUNC, i, size());
            return;
        }
        char* stringPoolIndices = strings_[i].c_str();
        int len = sizes_[i];
        for (int i = 0; i < len; ++i) {
            int stringPoolIdx = stringPoolIndices[i];
            stringPool_->deallocate_idx(stringPoolIdx);
        }
        strings_[i] = "";
        sizes_[i] = 0;
    }

    /*!
        @brief  Pushes a string to the back of the ith deque

        @param  i
                The deque index.

        @param  str
                The string to push to the back of the deque.
    */
    void push_back(int const & i, PoolString<StringPool> const & str) {
        Log.trace(F("DequeDequePoolString::push_back(%d, %s)\n"), i, str.c_str());
        if (i < 0 || i >= size()) {
            Log.warning(F("DequeDequePoolString::push_back accessing index i = %d when size is %d\n"), i, size());
            return;
        }
        int stringPoolIdx = stringPool_->allocate_idx();
        stringPool_->strcpy(stringPoolIdx, str.c_str());
        char str_[2] = " ";
        str_[0] = char(stringPoolIdx);
        strings_[i] += str_;
        sizes_[i] += 1;
        Log.trace(F("DequeDequePoolString::push_back %s given stringPoolIdx %d\n"), str.c_str(), stringPoolIdx);
    }

private:
    StringPool * stringPool_;

    Deque<poolstring_t, Alloc> strings_;
    Deque<int, Alloc>          sizes_;
    
};

} // namespace kty
