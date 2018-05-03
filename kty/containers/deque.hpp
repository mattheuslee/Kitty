#pragma once

#include <kty/containers/allocator.hpp>

namespace kty {

/*!
    @brief  Double-ended queue.
            Provides quick insertion and deletion at both ends,
            but at the expense of slow random access. 
            Implemented as a circular doubly linked list with a dummy head node.
*/
template <typename T>
class Deque {

public:
    /*!
        @brief  The node structure that makes up the deque.
    */
    struct DequeNode {
        /** The value stored in the node */
        T value;
        /** The node after this one */
        DequeNode* next;
        /** The node before this one */
        DequeNode* prev;
    };

    /*!
        @brief  Constructor for the deque.

        @param  allocator
                The allocator for the deque nodes.
    */
    Deque(Allocator<DequeNode>& allocator) : allocator_(allocator) {
        head = allocator_.allocate();
        head->next = head;
        head->prev = head;
    }

    /*!
        @brief  Destructor for the deque.
    */
    ~Deque() {
        while (!is_empty()) {
            pop_front();
        }
        allocator_.deallocate(head);
        head = NULL;
    }

    /*!
        @brief  Returns true if the deque is empty, false otherwise.

        @return True if the deque is empty, false otherwise.
    */
    bool is_empty() {
        return head->next == head;
    }

    /*!
        @brief  Pushes a value to the front of the deque.

        @param  value
                The value to push to the front of the deque.
    */
    void push_front(T const & value) {
        // Allocate new node
        DequeNode* toInsert = allocator_.allocate();
        toInsert.value = value;
        DequeNode* next = head->next;
        // Rearrange pointers
        toInsert.next = next;
        toInsert.prev = head;
        next->prev = toInsert;
        head->next = toInsert;
    }

    /*!
        @brief  Pops value from the front of the deque.
                If there is no value to pop, does nothing.
    */
    void pop_front() {
        if (is_empty()) {
            return;
        }
        DequeNode* toRemove = head->next;
        DequeNode* next = toRemove->next;
        // Rearrange pointers to bypass node to be removed
        head->next = next;
        next->prev = head;
        allocator_.deallocate(toRemove);
    }

    /*!
        @brief  Pushes a value to the back of the deque.

        @param  value
                The value to push to the back of the deque.
    */
    void push_back(T const & value) {
        // Allocate new node
        DequeNode* toInsert = allocator_.allocate();
        toInsert.value = value;
        DequeNode* prev = head->prev;
        // Rearrange pointers
        toInsert.next = head;
        toInsert.prev = prev;
        prev->next = toInsert;
        head->prev = toInsert;
    }

    /*!
        @brief  Pops value from the back of the deque.
                If there is no value to pop, does nothing.
    */
    void pop_back() {
        if (is_empty()) {
            return;
        }
        DequeNode* toRemove = head->prev;
        DequeNode* prev = toRemove->prev;
        // Rearrange pointers to bypass node to be removed
        head->prev = prev;
        prev->next = head;
        allocator_.deallocate(toRemove);
    }

    /*!
        @brief  Returns a reference to the front element of the deque.
                Has undefined behaviour if the deque is empty.
                
        @return A reference to the element.
    */
    T& front() {
        return head->next->value;
    }

    /*!
        @brief  Returns a reference to the back element of the deque.
                Has undefined behaviour if the deque is empty.

        @return A reference to the element.
    */
    T& back() {
        return head->prev->value;
    }

    /*!
        @brief  Returns a reference to the ith-indexed element of the deque.
                Has undefined behaviour if the deque has less than i elements. 

        @param  i
                The index of the element to retrieve.

        @return A reference to the element.
    */
    T& operator[](int const & i) {
        DequeNode* curr = head->next;
        for (int j = 0; j < i; ++j) {
            curr = curr->next;
        }
        return curr->value;
    }

private:
    DequeNode* head;

    Allocator<DequeNode>& allocator_;

};

} // namespace kty
