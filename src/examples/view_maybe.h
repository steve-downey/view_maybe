// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

template <class T>
struct dereference {
    typedef std::remove_reference_t<decltype(*std::declval<T>())> type;
};

template <class T>
using dereference_t = typename dereference<T>::type;

template<class T>
concept bool Dereferenceable =
    requires(T& t) {
    { *t } -> auto&&;
};


template <class T>
concept bool Readable =
    Dereferenceable<T> &&
    requires (T t) {
    t ? true : false;
};


template <Readable Maybe, typename T>
requires std::is_object_v<T>
class maybe_view;

template <Readable Maybe, typename T>
requires std::is_object_v<T>&&
std::is_rvalue_reference_v<Maybe>
class maybe_view<Maybe, T>
    : public std::experimental::ranges::view_interface<maybe_view<Maybe, T>> {
  private:
    using M = std::remove_cv_t<std::remove_reference_t<Maybe>>;

    std::experimental::ranges::detail::semiregular_box<M> value_;

  public:
    maybe_view() = default;

    constexpr maybe_view(Maybe maybe) : value_(std::move(maybe)) {}
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

template <Readable Maybe, typename T>
requires std::is_object_v<T>&&
std::is_lvalue_reference_v<Maybe>
class maybe_view<Maybe, T> {
    Maybe& value_;
    using R = std::remove_reference_t<decltype(*value_)>;

  public:
    maybe_view() = default;

    constexpr maybe_view(Maybe& maybe) : value_(maybe) {}

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
        if (value_.get())
            return 1;
        else
            return 0;
    }

    constexpr R* data() noexcept {
        if (value_)
            return std::addressof(*value_);
        else
            return 0;
    }

    constexpr const R* data() const noexcept {
        if (value_)
            return std::addressof(*value_);
        else
            return 0;
    }
};


template <Readable Maybe>
maybe_view(const Maybe&)->maybe_view<const Maybe&, dereference_t<Maybe>>;

template <Readable Maybe>
maybe_view(Maybe &&)->maybe_view<Maybe&&, dereference_t<Maybe>>;

template <Readable Maybe>
maybe_view(Maybe&)->maybe_view<Maybe&, dereference_t<Maybe>>;

namespace view {
struct __maybe_fn {
    template <Readable T>
    constexpr auto operator()(T&& t) const
        STL2_NOEXCEPT_REQUIRES_RETURN(maybe_view{std::forward<T>(t)})
        };

inline constexpr __maybe_fn maybe{};
} // namespace view

#endif
