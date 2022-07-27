// concepts.h                                                         -*-C++-*-
#ifndef INCLUDED_CONCEPTS
#define INCLUDED_CONCEPTS

#include <concepts>
#include <type_traits>
#include <ranges>

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

template <class Ref, class ConstRef>
concept readable_references =
    std::is_lvalue_reference_v<Ref> &&
    std::is_object_v<std::remove_reference_t<Ref>> &&
    std::is_lvalue_reference_v<ConstRef> &&
    std::is_object_v<std::remove_reference_t<ConstRef>> &&
    std::convertible_to<std::add_pointer_t<ConstRef>,
                        const std::remove_reference_t<Ref>*>;

template <class T>
concept nullable_object = std::is_object_v<T> && requires(T& t, const T& ct) {
                                                     bool(t);
                                                     bool(ct);
                                                     *(t);
                                                     *(ct);
                                                 };

template <class T>
concept nullable_object_val =
    nullable_object<T> && readable_references<std::iter_reference_t<T>,
                                              std::iter_reference_t<const T>>;

template <typename, template <typename...> class>
inline constexpr bool is_v = false;
template <typename... Ts, template <typename...> class C>
inline constexpr bool is_v<C<Ts...>, C> = true;

template <class T>
concept nullable_object_ref =
    is_v<T, std::reference_wrapper> && nullable_object_val<typename T::type>;

template <class T>
inline constexpr bool is_reference_wrapper_v = is_v<T, std::reference_wrapper>;

template <class T>
concept copyable_object = (std::copy_constructible<T> && std::is_object_v<T>);

#endif
