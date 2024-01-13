// smd/views/concepts.h                                               -*-C++-*-
#ifndef INCLUDED_SMD_VIEWS_CONCEPTS
#define INCLUDED_SMD_VIEWS_CONCEPTS

#include <concepts>
#include <ranges>
#include <type_traits>

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

namespace smd::views {

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

template <typename T>
struct is_reference_wrapper : std::false_type {};
template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};
template <typename T>
inline constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;
template <typename T>
concept reference_wrapper = is_reference_wrapper_v<T>;

template <class T>
concept nullable_object_ref =
    reference_wrapper<T> && nullable_object_val<typename T::type>;

template <class T>
concept movable_object = (std::move_constructible<T> && std::is_object_v<T>);
} // namespace smd::views
#endif
