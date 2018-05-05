#pragma once

#include <kty/containers/allocator.hpp>

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
    Deque(Alloc & allocator) : allocator_(allocator) {
        size_ = 0;
        head_ = (Node*)allocator_.allocate();
        head_->next = head_;
        head_->prev = head_;
    }

    /*!
        @brief  Destructor for the deque.
    */
    virtual ~Deque() {
        while (!is_empty()) {
            pop_front();
        }
        allocator_.deallocate(head_);
        head_ = NULL;
    }

    /*!
        @brief  Returns the size of the deque.

        @return The number of elements in the deque.
    */
    virtual int size() {
        return size_;
    }

    /*!
        @brief  Returns true if the deque is empty, false otherwise.

        @return True if the deque is empty, false otherwise.
    */
    virtual bool is_empty() {
        return size_ == 0;
    }

    /*!
        @brief  Pushes a value to the front of the deque.

        @param  value
                The value to push to the front of the deque.
    */
    virtual void push_front(T const & value) {
        // Allocate new node
        Node* toInsert = (Node*)allocator_.allocate();
        toInsert->value = value;
        Node* next = head_->next;
        // Rearrange pointers
        toInsert->next = next;
        toInsert->prev = head_;
        next->prev = toInsert;
        head_->next = toInsert;
        ++size_;
    }

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
        allocator_.deallocate(toRemove);
        --size_;
    }

    /*!
        @brief  Pushes a value to the back of the deque.

        @param  value
                The value to push to the back of the deque.
    */
    virtual void push_back(T const & value) {
        // Allocate new node
        Node* toInsert = (Node*)allocator_.allocate();
        toInsert->value = value;
        Node* prev = head_->prev;
        // Rearrange pointers
        toInsert->next = head_;
        toInsert->prev = prev;
        prev->next = toInsert;
        head_->prev = toInsert;
        ++size_;
    }

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
        allocator_.deallocate(toRemove);
        --size_;
    }

    /*!
        @brief  Returns a reference to the front element of the deque.
                Has undefined behaviour if the deque is empty.
                
        @return A reference to the element.
    */
    virtual T& front() {
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
        Node* curr = head_->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        return curr->value;
    }

protected:
    /** Pointer to the head node of the internal linked list */
    Node* head_;
    /** Current size of the linked list */
    int size_;

    /** Reference to the allocator used to allocate new nodes */
    Alloc& allocator_;

};

} // namespace kty
