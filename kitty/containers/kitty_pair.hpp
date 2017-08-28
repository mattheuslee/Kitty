#pragma once

#ifndef KITTY_PAIR_HPP
#define KITTY_PAIR_HPP

namespace kitty {

template <class T1, class T2>
struct kitty_pair {
    T1 first;
    T2 second;
};

template <class T1, class T2>
kitty_pair<T1, T2> make_kitty_pair(T1 const & t1, T2 const & t2) {
    return kitty_pair<T1, T2>{t1, t2};
}

}

#endif // KITTY_PAIR_HPP
