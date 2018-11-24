// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

#include <experimental/ranges/concepts>
#include <iostream>


template<class> struct dereference_type {};

template<class D>
requires
requires(const D& d) {{ *d } -> auto&&; }
struct dereference_type<D> {
    using type = decltype(*std::declval<const D&>());
};

template<class D>
using dereference_t = typename dereference_type<D>::type;


template<class T>
concept bool ContextualBool =
    requires(const T& t) {
    {bool(t)};
};

template <class T>
concept bool Nullable =
    std::experimental::ranges::Readable<std::remove_reference_t<T>> &&
    ContextualBool<T> &&
    requires (const T& t) {
    typename dereference_t<T>;
    std::is_object_v<dereference_t<T>>;
};


template <Nullable Maybe>
class safe_maybe_view
    : public std::experimental::ranges::view_interface<safe_maybe_view<Maybe>> {
  private:
    using T = std::remove_reference_t<dereference_t<Maybe>>;
    using M = std::remove_cv_t<std::remove_reference_t<Maybe>>;

    std::experimental::ranges::detail::semiregular_box<M> value_;

  public:
    safe_maybe_view() = default;

    constexpr explicit safe_maybe_view(Maybe const& maybe) : value_(maybe) {}
    constexpr explicit safe_maybe_view(Maybe&& maybe) : value_(std::move(maybe)) {}
    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr const T* end() const noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr std::ptrdiff_t size() noexcept {
        if (value_.get())
            return 1;
        else
            return 0;
    }

    constexpr T* data() noexcept {
        if (value_.get())
            return std::addressof(*value_.get());
        else
            return 0;
    }

    constexpr const T* data() const noexcept {
        if (value_.get())
            return std::addressof(*value_.get());
        else
            return 0;
    }
};

template <Nullable Maybe>
class ref_maybe_view
    : public std::experimental::ranges::view_interface<ref_maybe_view<Maybe>> {
    std::remove_reference_t<Maybe>* value_;
    using R = std::remove_reference_t<decltype(**value_)>;

  public:
    constexpr ref_maybe_view() = default;
    constexpr ref_maybe_view(ref_maybe_view const&) = default;

    constexpr explicit ref_maybe_view(Maybe& maybe) : value_(std::addressof(maybe)) {}

    constexpr R*       begin() noexcept { return data(); }
    constexpr const R* begin() const noexcept { return data(); }
    constexpr R*       end() noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr const R* end() const noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr std::ptrdiff_t size() noexcept {
        if (**value_)
            return 1;
        else
            return 0;
    }

    constexpr R* data() noexcept {
        if (*value_)
            return std::addressof(**value_);
        else
            return nullptr;
    }

    constexpr const R* data() const noexcept {
        if (*value_)
            return std::addressof(**value_);
        else
            return nullptr;
    }
};

namespace view {
struct __maybe_fn {
    template <Nullable T>
    constexpr auto operator()(T&& t) const
        noexcept(noexcept(safe_maybe_view{std::forward<T>(t)}))
        requires std::is_rvalue_reference_v<T>
        && requires {
        safe_maybe_view{std::forward<T>(t)};
    }
    {
        return safe_maybe_view{std::forward<T>(t)};
    }

    template <Nullable T>
    constexpr auto operator()(T&& t) const
        requires !std::is_rvalue_reference_v<T>
        //     noexcept(noexcept(maybe_view{std::forward<T>(t)}))
        //     requires requires {
        //     maybe_view{std::forward<T>(t)};
        // }
    {
        if constexpr (std::is_reference_v<T>) {
            return ref_maybe_view{std::forward<T>(t)};
        } else {
            return safe_maybe_view{std::forward<T>(t)};
        }
    }
};

inline constexpr __maybe_fn maybe{};
} // namespace view

#endif
