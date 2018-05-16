#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/sizes.hpp>
#include <kty/types.hpp>

namespace kty {

/*!
    @brief  Double-ended queue.
            Provides quick insertion and deletion at both ends,
            but at the expense of slow random access. 
            Implemented as a circular doubly linked list with a dummy head node.
*/
template <typename T, typename Alloc = Allocator<Sizes::alloc_size, Sizes::alloc_block_size>, typename GetAllocFunc = decltype(get_alloc)>
class Deque {

public:
    /** The type of value stored in the deque */
    typedef T value_t;

    /*!
        @brief  The node structure that makes up the deque.
    */
    struct Node {
        /** The value stored in the node */
        value_t value;
        /** The node after this one */
        Node* next;
        /** The node before this one */
        Node* prev;
    };

    /*!
        @brief  The iterator class for the deque.
    */
    class Iterator {
    
    public:
        /** Friend class declaration such that deque can access its internal pointers */
        friend class Deque<value_t, Alloc>;

        /*!
            @brief  Constructor.

            @param  ptr
                    The pointer to store in the iterator.
        */
        explicit Iterator(Node * ptr)
            : ptr_(ptr) {
        }

        /*!
            @brief  Pre-decrement operator.

            @return A reference to the the iterator after decrementing.
        */
        Iterator& operator--() {
            ptr_ = ptr_->prev;
            return *this;
        }

        /*!
            @brief  Post-decrement operator.

            @return A copy of the iterator in its pre-decremented state.
        */
        Iterator operator--(int) {
            Iterator temp(ptr_);
            ptr_ = ptr_->prev;
            return temp;
        }

        /*!
            @brief  Pre-increment operator.
                    Stops once it exceeds the end of the deque.

            @return A reference to the the iterator after incrementing.
        */
        Iterator& operator++() {
            ptr_ = ptr_->next;
            return *this;
        }

        /*!
            @brief  Post-increment operator.
                    Stops once it exceeds the end of the deque.

            @return A copy of the iterator in its pre-incremented state.
        */
        Iterator operator++(int) {
            Iterator temp(ptr_);
            ptr_ = ptr_->next;
            return temp;
        }

        /*!
            @brief  Dereference operator.

            @return A reference to the value pointed to by the iterator.
        */
        value_t & operator*() {
            return ptr_->value;
        }

        /*!
            @brief  Arrow operator.

            @return A pointer to the value pointed to by the iterator.
        */
        value_t * operator->() {
            return &(ptr_->value);
        }

        /*!
            @brief  Equality comparison operator.

            @param  other
                    The other iterator to compare to.

            @return True if this and the other iterator point to the same value,
                    false otherwise.
        */
        bool operator==(Iterator const & other) const {
            return ptr_ == other.ptr_;
        }

        /*!
            @brief  Non-equality comparison operator.

            @param  other
                    The other iterator to compare to.

            @return True if this and the other iterator point to different values,
                    false otherwise.
        */
        bool operator!=(Iterator const & other) const {
            return !operator==(other);
        }

    private:
        Node* ptr_;
    };

    /*!
        @brief  The constant iterator class for the deque.
                This behaves similarly to a regular iterator, but
                the values it points to cannot be changed.
    */
    class ConstIterator {
    
    public:
        /** Friend class declaration such that deque can access internal pointers */
        friend class Deque<value_t, Alloc>;

        /*!
            @brief  Constructor.

            @param  ptr
                    The pointer to store in the iterator.
        */
        explicit ConstIterator(Node const * ptr)
            : ptr_(const_cast<Node *>(ptr)) {
        }

        /*!
            @brief  Pre-decrement operator.

            @return A reference to the the iterator after decrementing.
        */
        ConstIterator& operator--() {
            ptr_ = ptr_->prev;
            return *this;
        }

        /*!
            @brief  Post-decrement operator.

            @return A copy of the iterator in its pre-decremented state.
        */
        ConstIterator operator--(int) {
            Iterator temp(ptr_);
            ptr_ = ptr_->prev;
            return temp;
        }

        /*!
            @brief  Pre-increment operator.
                    Stops once it exceeds the end of the deque.

            @return A reference to the the iterator after incrementing.
        */
        ConstIterator& operator++() {
            ptr_ = ptr_->next;
            return *this;
        }

        /*!
            @brief  Post-increment operator.
                    Stops once it exceeds the end of the deque.

            @return A copy of the iterator in its pre-incremented state.
        */
        ConstIterator operator++(int) {
            Iterator temp(ptr_);
            ptr_ = ptr_->next;
            return temp;
        }

        /*!
            @brief  Dereference operator.

            @return A const reference to the value pointed to by the iterator.
        */
        value_t const & operator*() const {
            return ptr_->value;
        }

        /*!
            @brief  Arrow operator.

            @return A const pointer to the value pointed to by the iterator.
        */
        value_t const * operator->() const {
            return &(ptr_->value);
        }

        /*!
            @brief  Equality comparison operator.

            @param  other
                    The other iterator to compare to.

            @return True if this and the other iterator point to the same value,
                    false otherwise.
        */
        bool operator==(ConstIterator const & other) const {
            return ptr_ == other.ptr_;
        }

        /*!
            @brief  Non-equality comparison operator.

            @param  other
                    The other iterator to compare to.

            @return True if this and the other iterator point to different values,
                    false otherwise.
        */
        bool operator!=(ConstIterator const & other) const {
            return !operator==(other);
        }

    private:
        Node* ptr_;
    };

    /*!
        @brief  Constructor for the deque.
                This constructor sets up the deque to use an allocator.

        @param  allocator
                The allocator for the deque nodes.
    */
    explicit Deque(Alloc & allocator)
        : allocator_(&allocator) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        static_assert(sizeof(Node) <= Sizes::alloc_block_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_block_size, due to fixed allocator memory block size.");
        size_ = 0;
        head_ = alloc();
        head_->next = head_;
        head_->prev = head_;
    }

    /*!
        @brief  Constructor for the deque.
                This constructor sets up the deque to use allocating functions.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.
    */
    explicit Deque(GetAllocFunc & getAllocFunc = get_alloc) 
        : getAllocFunc_(&getAllocFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        static_assert(sizeof(Node) <= Sizes::alloc_block_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_block_size, due to fixed allocator memory block size.");
        size_ = 0;
        head_ = alloc();
        head_->next = head_;
        head_->prev = head_;
    }

    /*!
        @brief  Copy constructor for the deque.

        @param  other
                The deque to copy from.
    */
    Deque(Deque<value_t, Alloc> const & other) 
        : allocator_(other.allocator_), getAllocFunc_(other.getAllocFunc_) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        static_assert(sizeof(Node) <= Sizes::alloc_block_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_block_size, due to fixed allocator memory block size.");
        size_ = 0;
        head_ = alloc();
        head_->next = head_;
        head_->prev = head_;
        // Copy over nodes from other deque
        for (ConstIterator it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }

    /*!
        @brief  Copy assignment operator for the deque.

        @param  other
                The deque to copy from.

        @return A reference to this deque after the copy.
    */
    Deque<value_t, Alloc> & operator=(Deque<value_t, Alloc> const & other) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Clear our own nodes
        clear();
        dalloc(head_);
        // Restart our deque
        allocator_ = other.allocator_;
        getAllocFunc_ = other.getAllocFunc_;
        size_ = 0;
        head_ = alloc();
        head_->next = head_;
        head_->prev = head_;
        // Copy over nodes from other deque
        for (ConstIterator it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    /*!
        @brief  Destructor for the deque.
    */
    virtual ~Deque() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        clear();
        dalloc(head_);
    }

    /*!
        @brief  Performs the correct allocation depending on whether
                an allocator object is given, or if a function to get
                an allocator object is given.
        
        @return A pointer to the allocated node.
    */
    Node * alloc() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (allocator_ != nullptr) {
            Log.verbose(F("%s: allocator\n"), PRINT_FUNC);
            return static_cast<Node *>(allocator_->allocate());
        }
        else {
            Log.verbose(F("%s: getAllocFunc\n"), PRINT_FUNC);
            return static_cast<Node *>((*getAllocFunc_)(nullptr)->allocate());
        }
    }

    /*!
        @brief  Performs the correct deallocation depending on whether
                an allocator object is given, or if a function to get
                an allocator object is given.
        
        @param  ptr
                The pointer to deallocate.

        @return True if the deallocation was successful, false otherwise.
    */
    bool dalloc(Node * ptr) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (allocator_ != nullptr) {
            Log.verbose(F("%s: allocator\n"), PRINT_FUNC);
            return allocator_->deallocate(ptr);
        }
        else {
            Log.verbose(F("%s: getAllocFunc\n"), PRINT_FUNC);
            return (*getAllocFunc_)(nullptr)->deallocate(ptr);
        }
    }

    /*!
        @brief  Returns the size of the deque.

        @return The number of elements in the deque.
    */
    virtual int size() const {
        return size_;
    }

    /*!
        @brief  Returns true if the deque is empty, false otherwise.

        @return True if the deque is empty, false otherwise.
    */
    virtual bool is_empty() const {
        return size_ == 0;
    }

    /*!
        @brief  Clears all elements in the deque, leaving it empty.
    */
    virtual void clear() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        while (!is_empty()) {
            pop_front();
        }
    }

    /*!
        @brief  Pushes a value to the front of the deque.

        @param  value
                The value to push to the front of the deque.

        @return True if the push was successful, false otherwise.
    */
    virtual bool push_front(value_t const & value) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Allocate new node
        Node* toInsert = alloc();
        if (toInsert == nullptr) {
            Log.warning(F("%s: Unable to push back due to invalid allocated address\n"), PRINT_FUNC);
            return false;
        }
        toInsert->value = value;
        Node* next = head_->next;
        // Rearrange pointers
        toInsert->next = next;
        toInsert->prev = head_;
        next->prev = toInsert;
        head_->next = toInsert;
        ++size_;
        Log.verbose(F("%s: done\n"), PRINT_FUNC);
        return true;
    }

    /*!
        @brief  Pops value from the front of the deque.
                If there is no value to pop, does nothing.

        @return True if the push was successful, false otherwise.
    */
    virtual bool pop_front() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (is_empty()) {
            return false;
        }
        Node* toRemove = head_->next;
        Node* next = toRemove->next;
        // Rearrange pointers to bypass node to be removed
        head_->next = next;
        next->prev = head_;
        // Call destructor before deallocating memory
        toRemove->value.~value_t();
        bool result = dalloc(toRemove);
        --size_;
        Log.verbose(F("%s: done\n"), PRINT_FUNC);
        return result;
    }

    /*!
        @brief  Pushes a value to the back of the deque.
                This version calls the copy constructor.

        @param  value
                The value to push to the back of the deque.

        @return True if the push was successful, false otherwise.
    */
    virtual bool push_back(value_t const & value) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        // Allocate new node
        Node* toInsert = alloc();
        if (toInsert == nullptr) {
            Log.warning(F("%s: Unable to push back due to invalid allocated address\n"), PRINT_FUNC);
            return false;
        }
        toInsert->value = value_t(value);
        Node* prev = head_->prev;
        // Rearrange pointers
        toInsert->next = head_;
        toInsert->prev = prev;
        prev->next = toInsert;
        head_->prev = toInsert;
        ++size_;
        Log.verbose(F("%s: done\n"), PRINT_FUNC);
        return true;
    }

    /*!
        @brief  Pops value from the back of the deque.
                If there is no value to pop, does nothing.

        @return True if the push was successful, false otherwise.
    */
    virtual bool pop_back() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (is_empty()) {
            return false;
        }
        Node* toRemove = head_->prev;
        Node* prev = toRemove->prev;
        // Rearrange pointers to bypass node to be removed
        head_->prev = prev;
        prev->next = head_;
        // Call destructor before deallocating memory
        toRemove->value.~value_t();
        bool result = dalloc(toRemove);
        --size_;
        Log.verbose(F("%s: done\n"), PRINT_FUNC);
        return result;
    }

    /*!
        @brief  Returns a reference to the front element of the deque.
                Has undefined behaviour if the deque is empty.
                
        @return A reference to the element.
    */
    virtual value_t & front() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (size() == 0) {
            Log.warning(F("%s: size = 0 (undefined behaviour)\n"), PRINT_FUNC);
        }
        return head_->next->value;
    }

    /*!
        @brief  Returns a constant reference to the front element of the deque.
                Has undefined behaviour if the deque is empty.
                
        @return A reference to the element.
    */
    virtual value_t const & front() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (size() == 0) {
            Log.warning(F("%s: size = 0 (undefined behaviour)\n"), PRINT_FUNC);
        }
        return head_->next->value;
    }

    /*!
        @brief  Returns a reference to the back element of the deque.
                Has undefined behaviour if the deque is empty.

        @return A reference to the element.
    */
    virtual value_t & back() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (size() == 0) {
            Log.warning(F("%s: size = 0 (undefined behaviour)\n"), PRINT_FUNC);
        }        
        return head_->prev->value;
    }

    /*!
        @brief  Returns a constant reference to the back element of the deque.
                Has undefined behaviour if the deque is empty.

        @return A reference to the element.
    */
    virtual value_t const & back() const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (size() == 0) {
            Log.warning(F("%s: size = 0 (undefined behaviour)\n"), PRINT_FUNC);
        }        
        return head_->prev->value;
    }

    /*!
        @brief  Returns an iterator to the first element of the deque.

        @return An iterator to the first element.
    */
    virtual Iterator begin() {
        return Iterator(head_->next);
    }

    /*!
        @brief  Returns a const iterator to the first element of the deque.

        @return A const iterator to the first element.
    */
    virtual ConstIterator begin() const {
        return ConstIterator(head_->next);
    }

    /*!
        @brief  Returns a const iterator to the first element of the deque.

        @return A const iterator to the first element.
    */
    virtual ConstIterator cbegin() const {
        return ConstIterator(head_->next);
    }

    /*!
        @brief  Returns an iterator to one past the last element of the deque.
                Since this is a circular deque, one past the last element is the
                dummy head node.

        @return An iterator to one past the last element.
    */
    virtual Iterator end() {
        return Iterator(head_);
    }

    /*!
        @brief  Returns a const iterator to one past the last element of the deque.
                Since this is a circular deque, one past the last element is the
                dummy head node.

        @return A const iterator to one past the last element.
    */
    virtual ConstIterator end() const {
        return ConstIterator(head_);
    }

    /*!
        @brief  Returns a const iterator to one past the last element of the deque.
                Since this is a circular deque, one past the last element is the
                dummy head node.

        @return A const iterator to one past the last element.
    */
    virtual ConstIterator cend() const {
        return ConstIterator(head_);
    }

    /*!
        @brief  Erases the node at index.
        
        @param  idx
                The index of the node to erase.

        @return True if the erase was successful, false otherwise.
    */
    virtual bool erase(int const & idx) {
        if (idx >= size_) {
            Log.warning(F("%s: invalid idx %d to erase, size is %d\n"), PRINT_FUNC, idx, size_);
            return false;
        }
        Node* toRemove = head_->next;
        for (int i = 0; i < idx; ++i) {
            toRemove = toRemove->next;
        }
        Node* prev = toRemove->prev;
        Node* next = toRemove->next;
        // Redirect pointers
        prev->next = next;
        next->prev = prev;
        toRemove->value.~value_t();
        dalloc(toRemove);
        --size_;
        return true;
    }

    /*!
        @brief  Erases the node at the iterator, and returns an iterator to the
                node after the one which was erased.
        
        @param  it
                The iterator to the node to erase.
                This iterator will no longer be valid after this method call.
        
        @return An iterator to the node after the one which was erased.
    */
    virtual Iterator erase(Iterator const & it) {
        Node* toRemove = it.ptr_;
        Node* prev = toRemove->prev;
        Node* next = toRemove->next;
        // Redirect pointers
        prev->next = next;
        next->prev = prev;
        toRemove->value.~value_t();
        dalloc(toRemove);
        --size_;
        return Iterator(next);
    }

    /*!
        @brief  Returns a reference to the ith-indexed element of the deque.
                Has undefined behaviour if the deque has less than i elements. 

        @param  i
                The index of the element to retrieve.

        @return A reference to the element.
    */
    virtual value_t & operator[](int const & i) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (i < 0 || i >= size_) {
            Log.warning(F("%s: accessing index %d when size is %d (undefined behaviour)\n"), PRINT_FUNC, i, i, size_);
        }
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        Log.verbose(F("%s: returning\n"), PRINT_FUNC);
        return curr->value;
    }

    /*!
        @brief  Returns a constant reference to the ith-indexed element of the deque.
                Has undefined behaviour if the deque has less than i elements. 

        @param  i
                The index of the element to retrieve.

        @return A reference to the element.
    */
    virtual value_t const & operator[](int const & i) const {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        if (i < 0 || i >= size_) {
            Log.warning(F("%s: accessing index %d when size is %d (undefined behaviour)\n"), PRINT_FUNC, i, i, size_);
        }
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        Log.verbose(F("%s: returning\n"), PRINT_FUNC);
        return curr->value;
    }

protected:
    /** Pointer to the head node of the internal linked list */
    Node* head_ = nullptr;
    /** Current size of the linked list */
    int size_ = 0;

    /** Pointer to the allocator used to allocate new nodes */
    Alloc * allocator_ = nullptr;
    /** Pointer to the function to get the allocator */
    GetAllocFunc * getAllocFunc_ = nullptr;

};

} // namespace kty
