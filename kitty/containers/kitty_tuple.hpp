#pragma once

#ifndef KITTY_TUPLE_HPP_
#define KITTY_TUPLE_HPP_

template <typename T, typename... Ts>
struct kitty_tuple : public kitty_tuple<Ts...> {
    kitty_tuple(T t, Ts... ts) : kitty_tuple<Ts...>(ts...), value(t) {}
    T value;
};

template <typename T>
struct kitty_tuple<T> {
    kitty_tuple(T t) : value(t) {}
    T value;
};

template <int index, typename T, typename... Ts>
struct get_impl {
    static auto value(kitty_tuple<T, Ts...>* t) -> decltype(get_impl<index - 1, Ts...>::value(t)) {
		return get_impl<index - 1, Ts...>::value(t);
	}
};

template <int index, typename T, typename... Ts>
struct get_const_impl {
    static auto value(kitty_tuple<T, Ts...> const * t) -> decltype(get_const_impl<index - 1, Ts...>::value(t)) {
		return get_const_impl<index - 1, Ts...>::value(t);
	}
};

template <typename T, typename... Ts>
struct get_impl<0, T, Ts...> {
	static T& value(kitty_tuple<T, Ts...>* t) {
		return t->value;
	}
};

template <typename T, typename... Ts>
struct get_const_impl<0, T, Ts...> {
	static T value(kitty_tuple<T, Ts...> const * t) {
		return t->value;
	}
};

template <int index, typename T, typename... Ts>
auto get(kitty_tuple<T, Ts...> & t) -> decltype(get_impl<index, T, Ts...>::value(&t)) {
	return get_impl<index, T, Ts...>::value(&t);
}

template <int index, typename T, typename... Ts>
auto get(kitty_tuple<T, Ts...> const & t) -> decltype(get_const_impl<index, T, Ts...>::value(&t)) {
	return get_const_impl<index, T, Ts...>::value(&t);
}

#endif // KITTY_TUPLE_HPP_