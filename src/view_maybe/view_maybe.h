// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

#include <experimental/ranges/concepts>


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
    ContextualBool<T> &&
    requires (const T& t) {
    typename dereference_t<T>;
    std::is_object_v<dereference_t<T>>;
};


template <Nullable Maybe>
class maybe_view;

template <Nullable Maybe>
requires std::is_rvalue_reference_v<Maybe>
class maybe_view<Maybe>
    : public std::experimental::ranges::view_interface<maybe_view<Maybe>> {
  private:
    using T = std::remove_reference_t<dereference_t<Maybe>>;
    using M = std::remove_cv_t<std::remove_reference_t<Maybe>>;

    std::experimental::ranges::detail::semiregular_box<M> value_;

  public:
    maybe_view() = default;

    constexpr explicit maybe_view(Maybe maybe) : value_(std::move(maybe)) {}
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
requires std::is_lvalue_reference_v<Maybe>
class maybe_view<Maybe>
    : public std::experimental::ranges::view_interface<maybe_view<Maybe>> {
    std::remove_reference_t<Maybe>* value_;
    using R = std::remove_reference_t<decltype(**value_)>;

  public:
    maybe_view() = default;

    constexpr explicit maybe_view(Maybe& maybe) : value_(std::addressof(maybe)) {}
    constexpr maybe_view(maybe_view const&) = default;

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


template <Nullable Maybe>
maybe_view(const Maybe&)->maybe_view<const Maybe&>;

template <Nullable Maybe>
maybe_view(Maybe &&)->maybe_view<Maybe&&>;

template <Nullable Maybe>
maybe_view(Maybe&)->maybe_view<Maybe&>;


namespace view {
struct __maybe_fn {
    template <Nullable T>
    constexpr auto operator()(T&& t) const
        STL2_NOEXCEPT_REQUIRES_RETURN(maybe_view{std::forward<T>(t)})
        };

inline constexpr __maybe_fn maybe{};
} // namespace view

#endif
