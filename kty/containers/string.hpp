#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>

namespace kty {

/** Forward declaration necessary for StringDatabase friend class declaration */
template <class DB>
class DBString;

/*!
    @brief  Class that maintains all the strings in the program.
            The memory pool is created on the stack to avoid heap fragmentation.
            Holds enough memory to allocate N strings of at most size S.            
*/
template <int N, int S>
class StringDatabase {

public:
    /*!
        @brief  Constructor for the string database
    */
    StringDatabase() {
        memset((void*)pool_, '\0', N * (S + 1));
        memset((void*)taken_, false, N);
    }

    /*!
        @brief  Prints the addresses used by the string database
    */
    void dump_addresses() {
        Serial.print(F("StringDatabase: Pool addresses = "));
        Serial.print((unsigned int)pool_);
        Serial.print(F(" to "));
        Serial.println((unsigned int)(pool_ + N * (S + 1) - 1));
    }

    /*!
        @brief  Gets the next free index for a string.
    
        @return An index into the pool if there is space,
                -1 otherwise.
    */
    int allocate_idx() {
        for (int i = 0; i < N; ++i) {
            if (!taken_[i]) {
                taken_[i] = true;
                return i;
            }
        }
        Log.warning(F("StringDatabase: No more string indices to allocate\n"));
        return -1;
    }

    /*!
        @brief  Returns the maximum possible length for a string.

        @return The maximum possible string length.
    */
    int max_str_len() {
        return S;
    }

    /*!
        @brief  Returns the string at a given index.

        @param  idx
                The database index for the string.

        @return str
                The stored string.
    */
    char* get(int const & idx) {
        if (idx < N) {
            return pool_ + (idx * (S + 1));
        }
        Log.warning(F("StringDatabase: Attempt to get string at index %d, max index = %d\n"), idx, N - 1);
        return NULL;
    }

    /*!
        @brief  Sets a string in the database.

        @param  idx
                The database index for the string.

        @param  str
                The incoming string.
    */
    void set(int const & idx, char * str) {
        int len = strlen(str);
        int lenToCopy = S < len ? S : len;
        strncpy(pool_ + (idx * (S + 1)), str, lenToCopy);
    }

private:
    char pool_[N * (S + 1)];
    bool taken_[N];

};

/*!
    @brief  Class that interacts with the string database to produce
            string-like behaviour.
*/
template <class DataBase>
class DBString {

public:
    /*!
        @brief  Constructor for a database string.

        @param  database
                A reference to the database which this string is using to store the actual values.
    */
    DBString(DataBase & database) : database_(database) {
        databaseIdx_ = database.allocate_idx();
    }

    ~DBString() {

    }

    /*!
        @brief  Returns a string which is in "c-style". 
                This form is suitable for passing to Arduino serial print.
        
        @return A pointer to the first character in the string.
    */
    char* c_str() {
        return database_.get(databaseIdx_);
    }

    /*!
        @brief  Assigns a string value to this string.

        @param  str
                The string which will be assigned to this string.
    */
    void operator=(char * str) {
        database_.set(databaseIdx_, str);
    }

    /*!
        @brief  Copies another string to this string.

        @param  dbString
                The string to copy from.
    */
    void operator=(DBString & dbString) {
        Serial.println("operator=");
        database_.set(databaseIdx_, database_.get(dbString.databaseIdx_));
    }

private:
    int databaseIdx_;
    DataBase& database_;

};

} // namespace kty
