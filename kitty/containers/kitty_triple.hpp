#pragma once

#ifndef KITTY_TRIPLE_HPP
#define KITTY_TRIPLE_HPP

namespace kitty {

template <class T1, class T2, class T3>
struct kitty_triple {
    T1 first;
    T2 second;
    T3 third;
};

template <class T1, class T2, class T3>
kitty_triple<T1, T2, T3> make_kitty_triple(T1 const & t1, T2 const & t2, T3 const & t3) {
    return kitty_triple<T1, T2, T3>{t1, t2, t3};
}

}

#endif // KITTY_TRIPLE_HPP
