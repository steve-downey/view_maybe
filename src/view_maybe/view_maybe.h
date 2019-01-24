// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

#include <experimental/ranges/ranges>

namespace ranges = std::experimental::ranges;

//////////////////////////////////////////////////////////////
// Implementation

template <class T, class Ref, class ConstRef>
concept bool _Nullable2 =
    std::is_lvalue_reference_v<Ref> &&
    std::is_object_v<std::remove_reference_t<Ref>> &&
    std::is_lvalue_reference_v<ConstRef> &&
    std::is_object_v<std::remove_reference_t<ConstRef>> &&
    ranges::ConvertibleTo<std::add_pointer_t<ConstRef>,
                          const std::remove_reference_t<Ref>*>;

template <class T>
concept bool Nullable =
    std::is_object_v<T> &&
    requires(T& t, const T& ct) {
    bool(ct);
  *t;
  *ct;
} &&
_Nullable2<T, ranges::iter_reference_t<T>, ranges::iter_reference_t<const T>>;

template <Nullable Maybe>
class safe_maybe_view
    : public std::experimental::ranges::view_interface<safe_maybe_view<Maybe>> {
  private:
    using T = std::remove_reference_t<ranges::iter_reference_t<Maybe>>;

    std::experimental::ranges::detail::semiregular_box<Maybe> value_;

  public:
    constexpr safe_maybe_view() = default;
    constexpr explicit safe_maybe_view(Maybe const& maybe)
    noexcept(std::is_nothrow_copy_constructible_v<Maybe>)
    : value_(maybe) {}
    constexpr explicit safe_maybe_view(Maybe&& maybe)
    noexcept(std::is_nothrow_move_constructible_v<Maybe>)
    : value_(std::move(maybe)) {}

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept { return data() + size(); }
    constexpr const T* end() const noexcept { return data() + size(); }

    constexpr std::ptrdiff_t size() noexcept { return bool(value_.get()); }

    constexpr T* data() noexcept {
        Maybe& m = value_.get();
        if (m)
            return std::addressof(*m);
        else
            return nullptr;
    }
    constexpr const T* data() const noexcept {
        Maybe& m = value_.get();
        if (m)
            return std::addressof(*m);
        else
            return nullptr;
    }
};

template <Nullable Maybe>
class ref_maybe_view
    : public std::experimental::ranges::view_interface<ref_maybe_view<Maybe>> {
    using T = std::remove_reference_t<ranges::iter_reference_t<Maybe>>;

    Maybe* value_ = nullptr;

  public:
    constexpr ref_maybe_view() = default;
    constexpr explicit ref_maybe_view(Maybe& maybe) noexcept
    : value_(std::addressof(maybe)) {}

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept { return data() + size(); }
    constexpr const T* end() const noexcept { return data() + size(); }

    constexpr std::ptrdiff_t size() const noexcept { return bool(*value_); }

    constexpr T* data() noexcept {
        if (*value_)
            return std::addressof(**value_);
        else
            return nullptr;
    }
    constexpr const T* data() const noexcept {
        if (*value_)
            return std::addressof(**value_);
        else
            return nullptr;
    }
};

namespace view {
struct __maybe_fn {
    template <Nullable T>
    constexpr ref_maybe_view<T> operator()(T& t) const noexcept {
        return ref_maybe_view<T>{t};
    }

    template <class T, Nullable U = std::remove_cv_t<T>>
    requires ranges::Constructible<U, T>
    constexpr safe_maybe_view<U> operator()(T&& t) const
        noexcept(std::is_nothrow_constructible_v<U, T>)
    {
        return safe_maybe_view<U>{std::move(t)};
    }
};

inline constexpr __maybe_fn maybe{};
} // namespace view

#endif
