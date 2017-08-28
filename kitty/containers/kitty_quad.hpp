#pragma once

#ifndef KITTY_QUAD_HPP
#define KITTY_QUAD_HPP

namespace kitty {

template <class T1, class T2, class T3, class T4>
struct kitty_quad {
    T1 first;
    T2 second;
    T3 third;
    T4 fourth;
};

template <class T1, class T2, class T3, class T4>
kitty_quad<T1, T2, T3, T4> make_kitty_quad(T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4) {
    return kitty_quad<T1, T2, T3, T4>{t1, t2, t3, t4};
}

}

#endif // KITTY_QUAD_HPP
