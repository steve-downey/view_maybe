// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

#include <experimental/ranges/concepts>
#include <experimental/ranges/ranges>
#include <iostream>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/meta.hpp>

namespace ranges = std::experimental::ranges;

template <class Ref, class ConstRef>
concept bool readable_references =
    std::is_lvalue_reference_v<Ref> &&
    std::is_object_v<std::remove_reference_t<Ref>> &&
    std::is_lvalue_reference_v<ConstRef> &&
    std::is_object_v<std::remove_reference_t<ConstRef>> &&
    ranges::convertible_to<std::add_pointer_t<ConstRef>,
        const std::remove_reference_t<Ref>*>;

template <class T>
concept bool nullable =
    std::is_object_v<T> &&
    requires(T& t, const T& ct) {
    bool(ct);
  *(t);
  *(ct);
    };


template <class T>
concept bool nullable_val =
    nullable<T> &&
    readable_references<ranges::iter_reference_t<T>, ranges::iter_reference_t<const T>>;

template <class T>
concept bool nullable_ref =
    meta::is_v<T, std::reference_wrapper> &&
    nullable_val<typename T::type>;


// template<class T>
// struct maybe_unwrap : maybe_unwrap<std::decay_t<T>> {};

template<typename T>
struct maybe_unwrap { using type = T; };

template<class T>
struct maybe_unwrap<std::reference_wrapper<T>> { using type = T; };

template<class T>
inline constexpr bool is_reference_wrapper =
    meta::is_v<T, std::reference_wrapper>;

template <class Maybe>
    requires ranges::copy_constructible<Maybe> &&
(nullable_val<Maybe> ||
 nullable_ref<Maybe>)
class maybe_view
    : public std::experimental::ranges::view_interface<maybe_view<Maybe>> {
  private:
    using T = std::remove_reference_t<ranges::iter_reference_t<typename maybe_unwrap<Maybe>::type>>;

    std::experimental::ranges::detail::semiregular_box<Maybe> value_;

  public:
    constexpr maybe_view() = default;

    constexpr explicit maybe_view(Maybe const& maybe)
    noexcept(std::is_nothrow_copy_constructible_v<Maybe>)
        : value_(maybe) {}

    constexpr explicit maybe_view(Maybe&& maybe)
    noexcept(std::is_nothrow_move_constructible_v<Maybe>)
        : value_(std::move(maybe)) {}

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept { return data() + size(); }
    constexpr const T* end() const noexcept { return data() + size(); }

    constexpr std::ptrdiff_t size() const noexcept {
        if constexpr (is_reference_wrapper<Maybe>) {
            return bool(value_.get().get());
        } else {
            return bool(value_.get());
        }
    }

    constexpr T* data() noexcept {
        Maybe& m = value_.get();
        if constexpr (is_reference_wrapper<Maybe>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

    constexpr const T* data() const noexcept {
        const Maybe& m = value_.get();
        if constexpr (is_reference_wrapper<Maybe>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }
};



namespace views {
    struct __maybe_fn {
        template <typename T>
        constexpr auto operator()(T && t) const noexcept
        {
            return maybe_view{t};
        }
    };

    inline constexpr __maybe_fn maybe{};
} // namespace views

#endif
