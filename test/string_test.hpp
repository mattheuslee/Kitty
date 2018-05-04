#pragma once

#include <kty/stl_impl.hpp>
#include <cstring>
#include <vector>

#include <kty/containers/string.hpp>

using namespace std;
using namespace kty;

test(string_stringdatabase_dbstring)
{
    const int numStrings = 10;
    StringDatabase<numStrings, 20> stringDB;

    typedef DBString<decltype(stringDB)> DbString;
    
    vector<DbString> strings;

    for (int i = 0; i < numStrings; ++i) {
        strings.push_back(DbString(stringDB));
    }
    assertTrue(stringDB.allocate_idx() == -1);

    char str[20] = "";

    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        strings[i] = str;
    }

    for (int i = 0; i < numStrings; ++i) {
        str[0] = '0' + i;
        assertTrue(strcmp(strings[i].c_str(), str) == 0);
    }
}



