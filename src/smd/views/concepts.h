// smd/views/concepts.h                                               -*-C++-*-
#ifndef INCLUDED_SMD_VIEWS_CONCEPTS
#define INCLUDED_SMD_VIEWS_CONCEPTS

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

namespace smd {
template <class T>
concept maybe = requires(const T t) {
    bool(t);
    *(t);
};

namespace views {

template <class Ref, class ConstRef>
concept readable_references =
    std::is_lvalue_reference_v<Ref> &&
    std::is_object_v<std::remove_reference_t<Ref>> &&
    std::is_lvalue_reference_v<ConstRef> &&
    std::is_object_v<std::remove_reference_t<ConstRef>> &&
    std::convertible_to<std::add_pointer_t<ConstRef>,
                        const std::remove_reference_t<Ref>*>;

template <class T>
concept nullable_object = std::is_object_v<T> && smd::maybe<T> &&
                          readable_references<std::iter_reference_t<T>,
                                              std::iter_reference_t<const T>>;

template <class T>
concept movable_object = (std::move_constructible<T> && std::is_object_v<T>);
} // namespace views
} // namespace smd
#endif
