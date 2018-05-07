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
        Log.trace(F("Deque<%s>::constructor\n"), TYPE_STR(T));
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

    /*!
        @brief  Copy constructor for the deque.

        @param  other
                The other deque to copy from.
    */
    Deque(Deque<T, Alloc> const & other) : allocator_(other.allocator_) {
        Log.trace(F("Deque<%s>::copy constructor\n"), TYPE_STR(T));
        static_assert(sizeof(Node) <= Sizes::alloc_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_size, due to fixed allocator memory block size.");
        operator=(other);
    }

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

    /*!
        @brief  Copy assignment operator for deque
                Will perform copy construction on all elements.

        @param  other
                The deque to copy elements from.
    */
    void operator=(Deque<T, Alloc> const & other) {
        Log.trace(F("Deque<%s>::copy=\n"), TYPE_STR(T));
        // Only clear if we are starting with a properly initialised deque
        if (size_ >= 0 && allocator_ != nullptr && allocator_->owns(head_)) {
            Log.trace(F("Deque<%s>::copy= properly initialised deque\n"), TYPE_STR(T));
            clear();
            allocator_->deallocate(head_);
        }
        else {
            Log.trace(F("Deque<%s>::copy= non-initialised deque\n"), TYPE_STR(T));            
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
            Log.trace(F("Deque<%s>::copy= i = %d\n"), TYPE_STR(T), i);
            push_back(other[i]);
        }
        Log.trace(F("Deque<%s>::copy= done, size = %d\n"), TYPE_STR(T), size_);
    }

    /*!
        @brief  Destructor for the deque.
    */
    virtual ~Deque() {
        Log.trace(F("Deque<%s>::destructor\n"), TYPE_STR(T));
        // Only clear if we are starting with a properly initialised deque
        if (size_ >= 0 && allocator_ != nullptr && allocator_->owns(head_)) {
            Log.trace(F("Deque<%s>::destructor properly initialised deque\n"), TYPE_STR(T));
            clear();
            allocator_->deallocate(head_);
        }
        else {
            Log.trace(F("Deque<%s>::destructor non-initialised deque\n"), TYPE_STR(T));            
        }
    }

    /*!
        @brief  Returns the size of the deque.

        @return The number of elements in the deque.
    */
    virtual int size() const {
        Log.trace(F("Deque<%s>::size = %d\n"), TYPE_STR(T), size_);
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
        Log.trace(F("Deque<%s>::clear\n"), TYPE_STR(T));
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
        Log.trace(F("Deque<%s>::push_front copy\n"), TYPE_STR(T));
        //Log.trace(F("Deque::push_front copy\n"));
        // Allocate new node
        Node* toInsert = (Node*)(allocator_->allocate());
        toInsert->value = value;
        Node* next = head_->next;
        // Rearrange pointers
        toInsert->next = next;
        toInsert->prev = head_;
        next->prev = toInsert;
        head_->next = toInsert;
        ++size_;
        Log.trace(F("Deque<%s>::push_front copy done\n"), TYPE_STR(T));
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
    }

    /*!
        @brief  Pushes a value to the back of the deque.
                This version calls the copy constructor.

        @param  value
                The value to push to the back of the deque.
    */
    virtual void push_back(T const & value) {
        // Allocate new node
        Node* toInsert = (Node*)(allocator_->allocate());
        toInsert->value = T(value);
        Node* prev = head_->prev;
        // Rearrange pointers
        toInsert->next = head_;
        toInsert->prev = prev;
        prev->next = toInsert;
        head_->prev = toInsert;
        ++size_;
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
    }

    /*!
        @brief  Returns a reference to the front element of the deque.
                Has undefined behaviour if the deque is empty.
                
        @return A reference to the element.
    */
    virtual T& front() {
        Log.trace(F("Deque<%s>::front size = %d\n"), TYPE_STR(T), size());
        if (size() == 0) {
            Log.warning(F("Deque<%s>::front but size = 0 (undefined behaviour)\n"), TYPE_STR(T));
        }
        return head_->next->value;
    }

    /*!
        @brief  Returns a reference to the back element of the deque.
                Has undefined behaviour if the deque is empty.

        @return A reference to the element.
    */
    virtual T& back() {
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
        Log.trace(F("Deque::ref[%d]\n"), i);
        if (i < 0 || i >= size_) {
            Log.warning(F("Deque::ref[] accessing index %d when size is %d (undefined behaviour)\n"), i, size_);
        }
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            Log.trace(F("Deque::ref[%d] j = %d\n"), i, j);
            curr = curr->next;
        }
        Log.trace(F("Deque::ref[%d] returning\n"), i);
        return curr->value;
    }

    /*!
        @brief  Returns a copy to the ith-indexed element of the deque.
                Has undefined behaviour if the deque has less than i elements. 

        @param  i
                The index of the element to retrieve.

        @return A reference to the element.
    */
    virtual T operator[](int const & i) const {
        Log.trace(F("Deque::copy[%d]\n"), i);
        if (i < 0 || i >= size_) {
            Log.warning(F("Deque::copy[] accessing index %d when size if %d (undefined behaviour)\n"), i, size_);
        }
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        return curr->value;
    }

protected:
    /** Pointer to the head node of the internal linked list */
    Node* head_ = nullptr;
    /** Current size of the linked list */
    int size_ = 0;

    /** Reference to the allocator used to allocate new nodes */
    Alloc* allocator_;

};

} // namespace kty
