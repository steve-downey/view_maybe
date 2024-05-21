// smd/maybe.h                                               -*-C++-*-
#ifndef INCLUDED_SMD_MAYBE
#define INCLUDED_SMD_MAYBE

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

#include <optional>
#include <iterator>

namespace smd {
template <class T>
concept maybe = requires(const T t) {
    bool(t);
    *(t);
};

template <class T>
constexpr auto yield_if(bool b, T&& t) -> std::optional<std::decay_t<T>>;

template <smd::maybe T, class U, class R>
auto reference_or(T&& m, U&& u) -> R;

template <smd::maybe T, class U>
auto value_or();

template <class T, class I>
auto or_invoke();

} // namespace smd

template <class T>
constexpr auto smd::yield_if(bool b, T&& t) -> std::optional<std::decay_t<T>> {
    using O = std::optional<std::decay_t<T>>;
    return b ? std::forward<T>(t) : O{};
}

template <smd::maybe T,
          class U,
          class R = std::common_reference_t<std::iter_reference_t<T>, U&&>>
auto smd::reference_or(T&& m, U&& u) -> R {
#ifdef __cpp_lib_reference_from_temporary
    static_assert(!std::reference_constructs_from_temporary_v<R, U>);
    static_assert(!std::reference_constructs_from_temporary_v<R, T&>);
#endif
    return bool(m) ? static_cast<R>(*m) : static_cast<R>((U&&)u);
}

#endif
