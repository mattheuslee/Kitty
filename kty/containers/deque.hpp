#pragma once

#include <kty/stl_impl.hpp>
#include <utility>

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
template <typename T, typename Alloc>
class Deque {

public:
    /*!
        @brief  The node structure that makes up the deque.
    */
    struct Node {
        /** The value stored in the node */
        T value;
        /** The node after this one */
        Node* next;
        /** The node before this one */
        Node* prev;
    };

    /*!
        @brief  Constructor for the deque.

        @param  allocator
                The allocator for the deque nodes.
    */
    Deque(Alloc & allocator) : allocator_(&allocator) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        static_assert(sizeof(Node) <= Sizes::alloc_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_size, due to fixed allocator memory block size.");
        size_ = 0;
        head_ = (Node*)(allocator_->allocate());
        head_->next = head_;
        head_->prev = head_;
    }

#if 0
    /*!
        @brief  Move constructor for the deque.

        @param  other
                The other deque to move from.
    */
    Deque(Deque<T, Alloc> && other) : allocator_(other.allocator_) {
        Log.trace(F("Deque::move constructor\n"));
        static_assert(sizeof(Node) <= Sizes::alloc_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_size, due to fixed allocator memory block size.");
        operator=((Deque<T, Alloc>&&)other);
    }
#endif

#if 0
    /*!
        @brief  Copy constructor for the deque.

        @param  other
                The other deque to copy from.
    */
    Deque(Deque<T, Alloc> const & other) : allocator_(other.allocator_) {
        Log.trace(F("%s\n"), PRINT_FUNC);
        static_assert(sizeof(Node) <= Sizes::alloc_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_size, due to fixed allocator memory block size.");
        size_ = 0;
        head_ = (Node*)(allocator_->allocate());
        head_->next = head_;
        head_->prev = head_;
        // Create list by copying from other list
        for (int i = 0; i < other.size(); ++i) {
            push_back(other[i]);
        }
    }
#endif

#if 0
    /*!
        @brief  Move assignment operator for deque

        @param  other
                The deque to move elements from.
    */
    void operator=(Deque<T, Alloc> && other) {
        Log.trace(F("Deque::move assignment\n"));
        // Only clear if we are starting with a properly initialised deque
        if (size_ > 0 && allocator_->owns(head_)) {
            Log.trace(F("Deque::move assignment properly initialised deque\n"));
            clear();
        }
        // Reset pointers
        size_ = 0;
        if (allocator_->owns(head_)) {
            allocator_->deallocate(head_);
        }
        allocator_ = other.allocator_;
        // "Steal" entire list from other deque
        head_ = other.head_;
        // Let the other deque have their head
        other.head_ = (Node*)(other.allocator_->allocate());
        other.head_->next = other.head_;
        other.head_->prev = other.head_;
        other.size_ = 0;
    }
#endif

#if 0
    /*!
        @brief  Copy assignment operator for deque
                Will perform copy construction on all elements.

        @param  other
                The deque to copy elements from.
    */
    void operator=(Deque<T, Alloc> const & other) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        // Only clear if we are starting with a properly initialised deque
        if (size_ >= 0 && allocator_ != nullptr && allocator_->owns(head_)) {
            Log.trace(F("%s: properly initialised deque\n"), PRINT_FUNC);
            clear();
            allocator_->deallocate(head_);
        }
        else {
            Log.trace(F("%s: non-initialised deque\n"), PRINT_FUNC);            
        }
        // Reset
        size_ = 0;
        head_ = nullptr;
        // Create new head
        allocator_ = other.allocator_;
        head_ = (Node*)(allocator_->allocate());
        head_->next = head_;
        head_->prev = head_;
        // Create list by copying from other list
        for (int i = 0; i < other.size(); ++i) {
            push_back(other[i]);
        }
        Log.trace(F("%s: done, size = %d\n"), PRINT_FUNC, size_);
    }
#endif

    /*!
        @brief  Destructor for the deque.
    */
    virtual ~Deque() {
        Log.trace(F("%s\n"), PRINT_FUNC);
        // Only clear if we are starting with a properly initialised deque
        if (size_ >= 0 && allocator_ != nullptr && allocator_->owns(head_)) {
            Log.trace(F("%s: properly initialised deque\n"), PRINT_FUNC);
            clear();
            allocator_->deallocate(head_);
        }
    }

    /*!
        @brief  Returns the size of the deque.

        @return The number of elements in the deque.
    */
    virtual int size() const {
        //Log.trace(F("%s: size = %d\n"), PRINT_FUNC, size_);
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
        Log.trace(F("%s:\n"), PRINT_FUNC);
        while (!is_empty()) {
            pop_front();
        }
    }

    /*!
        @brief  Pushes a value to the front of the deque.

        @param  value
                The value to push to the front of the deque.
    */
    virtual void push_front(T const & value) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        // Allocate new node
        Node* toInsert = (Node*)(allocator_->allocate());
        if (toInsert == nullptr) {
            Log.warning(F("%s: Unable to push back due to invalid allocated address\n"), PRINT_FUNC);
            return;
        }
        toInsert->value = value;
        Node* next = head_->next;
        // Rearrange pointers
        toInsert->next = next;
        toInsert->prev = head_;
        next->prev = toInsert;
        head_->next = toInsert;
        ++size_;
        Log.trace(F("%s: done\n"), PRINT_FUNC);
    }

#if 0
    /*!
        @brief  Pushes a value to the front of the deque.
                This version calls the move constructor.

        @param  value
                The value to push to the front of the deque.
    */
    virtual void push_front(T && value) {
        Log.trace(F("Deque::push_front move\n"));
        // Allocate new node
        Node* toInsert = (Node*)(allocator_->allocate());
        toInsert->value = T((T&&)value);
        Node* next = head_->next;
        // Rearrange pointers
        toInsert->next = next;
        toInsert->prev = head_;
        next->prev = toInsert;
        head_->next = toInsert;
        ++size_;
    }
#endif

    /*!
        @brief  Pops value from the front of the deque.
                If there is no value to pop, does nothing.
    */
    virtual void pop_front() {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        if (is_empty()) {
            return;
        }
        Node* toRemove = head_->next;
        Node* next = toRemove->next;
        // Rearrange pointers to bypass node to be removed
        head_->next = next;
        next->prev = head_;
        // Call destructor before deallocating memory
        toRemove->value.~T();
        allocator_->deallocate(toRemove);
        --size_;
        Log.trace(F("%s: done\n"), PRINT_FUNC);
    }

    /*!
        @brief  Pushes a value to the back of the deque.
                This version calls the copy constructor.

        @param  value
                The value to push to the back of the deque.
    */
    virtual void push_back(T const & value) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        // Allocate new node
        Node* toInsert = (Node*)(allocator_->allocate());
        if (toInsert == nullptr) {
            Log.warning(F("%s: Unable to push back due to invalid allocated address\n"), PRINT_FUNC);
            return;
        }
        toInsert->value = T(value);
        Node* prev = head_->prev;
        // Rearrange pointers
        toInsert->next = head_;
        toInsert->prev = prev;
        prev->next = toInsert;
        head_->prev = toInsert;
        ++size_;
        Log.trace(F("%s: done\n"), PRINT_FUNC);
    }

#if 0
    /*!
        @brief  Pushes a value to the back of the deque.
                This version calls the move constructor.

        @param  value
                The value to push to the back of the deque.
    */
    virtual void push_back(T && value) {
        // Allocate new node
        Node* toInsert = (Node*)(allocator_->allocate());
        toInsert->value = T((T&&)value);
        Node* prev = head_->prev;
        // Rearrange pointers
        toInsert->next = head_;
        toInsert->prev = prev;
        prev->next = toInsert;
        head_->prev = toInsert;
        ++size_;
    }
#endif

    /*!
        @brief  Pops value from the back of the deque.
                If there is no value to pop, does nothing.
    */
    virtual void pop_back() {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        if (is_empty()) {
            return;
        }
        Node* toRemove = head_->prev;
        Node* prev = toRemove->prev;
        // Rearrange pointers to bypass node to be removed
        head_->prev = prev;
        prev->next = head_;
        // Call destructor before deallocating memory
        toRemove->value.~T();
        allocator_->deallocate(toRemove);
        --size_;
        Log.trace(F("%s: done\n"), PRINT_FUNC);
    }

    /*!
        @brief  Returns a reference to the front element of the deque.
                Has undefined behaviour if the deque is empty.
                
        @return A reference to the element.
    */
    virtual T& front() {
        Log.trace(F("%s:\n"), PRINT_FUNC);
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
    virtual T& back() {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        if (size() == 0) {
            Log.warning(F("%s: size = 0 (undefined behaviour)\n"), PRINT_FUNC);
        }        
        return head_->prev->value;
    }

    /*!
        @brief  Returns a reference to the ith-indexed element of the deque.
                Has undefined behaviour if the deque has less than i elements. 

        @param  i
                The index of the element to retrieve.

        @return A reference to the element.
    */
    virtual T& operator[](int const & i) {
        //Log.trace(F("%s:\n"), PRINT_FUNC, i);
        if (i < 0 || i >= size_) {
            Log.warning(F("%s: accessing index %d when size is %d (undefined behaviour)\n"), PRINT_FUNC, i, i, size_);
        }
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        //Log.trace(F("%s: returning\n"), PRINT_FUNC, i);
        return curr->value;
    }

#if 0
    /*!
        @brief  Returns a copy to the ith-indexed element of the deque.
                Has undefined behaviour if the deque has less than i elements. 

        @param  i
                The index of the element to retrieve.

        @return A reference to the element.
    */
    virtual T operator[](int const & i) const {
        //Log.trace(F("%s:\n"), PRINT_FUNC, i);
        if (i < 0 || i >= size_) {
            Log.warning(F("%s: accessing index %d when size if %d (undefined behaviour)\n"), PRINT_FUNC, i, i, size_);
        }
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        //Log.trace(F("%s: returning\n"), PRINT_FUNC, i);
        return curr->value;
    }
#endif

protected:
    /** Pointer to the head node of the internal linked list */
    Node* head_ = nullptr;
    /** Current size of the linked list */
    int size_ = 0;

    /** Reference to the allocator used to allocate new nodes */
    Alloc* allocator_;

};

} // namespace kty
