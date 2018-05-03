#pragma once

namespace kty {

/*!
    @brief  Class that performs allocation and deallocation of memory.
            Utilises a centrally managed pool to avoid heap fragmentation.
            Templated on a type T.
*/
template <typename T>
class Allocator {

public:
    /*!
        @brief  Constructor for the allocator.

        @param  poolSize
                The size of pool to prepare, in terms of number of items.

        @param  itemSize
                The size of each item, in bytes.
                Defaults to sizeof(T) if not provided.
    */
    Allocator(int const & poolSize, int const & itemSize = sizeof(T)) : poolSize_(poolSize), itemSize_(itemSize) {
        pool_ = (T**)malloc(sizeof(T*) * poolSize_);
        taken_ = (bool*)malloc(sizeof(bool) * poolSize_);
        T* ptr = NULL;
        for (int i = 0; i < poolSize_; ++i) {
            ptr = (T*)(malloc(itemSize));
            if (ptr == NULL) {
                Log.warning(F("Allocator received NULL when attempting to create pool\n"));
            }
            pool_[i] = ptr;
            taken_[i] = false;
        }
    }

    /*!
        @brief  Destructor for the allocator.
    */
    ~Allocator() {
        for (int i = 0; i < poolSize_; ++i) {
            free(pool_[i]);
            pool_[i] = NULL;
        }
        free(pool_);
        free(taken_);
        pool_ = NULL;
    }

    /*!
        @brief  Allocates a single block of memory from the pool.

        @return A pointer to a block of memory.
                If no memory is available, NULL is returned.
    */
    T* allocate() {
        for (int i = 0; i < poolSize_; ++i) {
            if (!taken_[i]) {
                taken_[i] = true;
                return pool_[i];
            }
        }
        Log.warning(F("Allocator could not allocate new block from pool\n"));
        return NULL;
    }

    /*!
        @brief  Returns a single block of memory to the pool.
                This does not call the destructor on the data stored at the block.

        @param  ptr
                A pointer to a block of memory to be returned to the pool.
    */
    void deallocate(T* ptr) {
        for (int i = 0; i < poolSize_; ++i) {
            if (pool_[i] == ptr) {
                taken_[i] = false;
                return;
            }
        }
        Log.warning(F("Pointer given to allocator to deallocate did not come from pool\n"));
    }

private:
    const int poolSize_;
    const int itemSize_;

    T** pool_;
    bool* taken_;

};

} // namespace kty
