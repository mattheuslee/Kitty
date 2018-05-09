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
        @brief  The iterator class for the deque.
    */
    class Iterator {
    
    public:
        /*!
            @brief  Constructor.

            @param  ptr
                    The pointer to store in the iterator.
        */
        Iterator(Node * & ptr) {
            ptr_ = ptr;
            if (ptr == nullptr) {
                Log.warning(F("%s: given nullptr\n"), PRINT_FUNC);
            }
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
        T& operator*() {
            return ptr_->value;
        }

        /*!
            @brief  Equality comparison operator.

            @param  other
                    The other iterator to compare to.

            @return True if this and the other iterator point to the same value,
                    false otherwise.
        */
        bool operator==(Iterator const & other) {
            return ptr_ == other.ptr_;
        }

        /*!
            @brief  Non-equality comparison operator.

            @param  other
                    The other iterator to compare to.

            @return True if this and the other iterator point to different values,
                    false otherwise.
        */
        bool operator!=(Iterator const & other) {
            return !operator==(other);
        }

    private:
        Node* ptr_;
    };

    /*!
        @brief  Constructor for the deque.

        @param  allocator
                The allocator for the deque nodes.
    */
    explicit Deque(Alloc & allocator) : allocator_(&allocator) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        static_assert(sizeof(Node) <= Sizes::alloc_size, "Size of Deque<T, Alloc>::Node can be no larger than kty::Sizes::alloc_size, due to fixed allocator memory block size.");
        size_ = 0;
        head_ = static_cast<Node*>(allocator_->allocate());
        head_->next = head_;
        head_->prev = head_;
    }

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

        @return True if the push was successful, false otherwise.
    */
    virtual bool push_front(T const & value) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        // Allocate new node
        Node* toInsert = static_cast<Node*>(allocator_->allocate());
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
        Log.trace(F("%s: done\n"), PRINT_FUNC);
        return true;
    }

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

        @return True if the push was successful, false otherwise.
    */
    virtual bool push_back(T const & value) {
        Log.trace(F("%s:\n"), PRINT_FUNC);
        // Allocate new node
        Node* toInsert = static_cast<Node*>(allocator_->allocate());
        if (toInsert == nullptr) {
            Log.warning(F("%s: Unable to push back due to invalid allocated address\n"), PRINT_FUNC);
            return false;
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
        return true;
    }

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
        @brief  Returns an iterator to the first element of the deque.

        @return An iterator to the first element.
    */
    virtual Iterator begin() {
        return Iterator(head_->next);
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

protected:
    /** Pointer to the head node of the internal linked list */
    Node* head_ = nullptr;
    /** Current size of the linked list */
    int size_ = 0;

    /** Reference to the allocator used to allocate new nodes */
    Alloc* allocator_;

};

} // namespace kty
