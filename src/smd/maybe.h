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

template <class T, class R>
constexpr auto yield_if(bool b, T&& t) -> R;

template <maybe T, class U, class R>
constexpr auto reference_or(T&& m, U&& u) -> R;

template <maybe T, class U, class R>
constexpr auto value_or(T&& m, U&& u) -> R;

template <maybe T, class I, class R>
constexpr auto or_invoke(T&& m, I&& invocable) -> R;

} // namespace smd

template <class T, class R = std::optional<std::decay_t<T>>>
constexpr auto smd::yield_if(bool b, T&& t) -> R {
    return b ? std::forward<T>(t) : R{};
}

template <smd::maybe T,
          class U,
          class R = std::common_reference_t<std::iter_reference_t<T>, U&&>>
constexpr auto smd::reference_or(T&& m, U&& u) -> R {
#ifdef __cpp_lib_reference_from_temporary
    static_assert(!std::reference_constructs_from_temporary_v<R, U>);
    static_assert(!std::reference_constructs_from_temporary_v<R, T&>);
#endif
    return bool(m) ? static_cast<R>(*m) : static_cast<R>((U&&)u);
}

template <smd::maybe T,
          class U,
          class R = std::common_type_t<std::iter_reference_t<T>, U&&>>
constexpr auto smd::value_or(T&& m, U&& u) -> R {
    return bool(m) ? static_cast<R>(*m) : static_cast<R>(std::forward<U>(u));
}

template <smd::maybe T,
          class I,
          class R = std::common_type_t<std::iter_reference_t<T>,
                                       std::invoke_result_t<I>>>
constexpr auto smd::or_invoke(T&& m, I&& invocable) -> R {
    return bool(m) ? static_cast<R>(*m) : static_cast<R>(invocable());
}

#endif
