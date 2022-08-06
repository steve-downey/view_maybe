// view_nullable.h                                                    -*-C++-*-
#ifndef INCLUDED_VIEW_NULLABLE
#define INCLUDED_VIEW_NULLABLE
#include <concepts>
#include <ranges>
#include <iostream>
#include <type_traits>
#include <functional>

#include <view_maybe/concepts.h>

namespace ranges = std::ranges;

template <typename T>
class nullable_view;


template <typename Value>
inline constexpr bool is_nullable_view_v = false;
template <typename Value>
inline constexpr bool is_nullable_view_v<nullable_view<Value>> = true;

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
class nullable_view<Nullable>
    : public ranges::view_interface<nullable_view<Nullable>> {
  private:
    using T = std::remove_reference_t<
        std::iter_reference_t<typename std::unwrap_reference_t<Nullable>>>;

    ranges::__detail::__box<Nullable> value_;

  public:
    constexpr nullable_view() = default;

    constexpr explicit nullable_view(Nullable const& nullable)
        : value_(nullable) {}

    constexpr explicit nullable_view(Nullable&& nullable)
        : value_(std::move(nullable)) {}

    template <class... Args>
        requires std::constructible_from<Nullable, Args...>
    constexpr nullable_view(std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...) {}

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept { return data() + size(); }
    constexpr const T* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept {
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return bool((*value_).get());
        } else {
            return bool(*value_);
        }
    }

    constexpr T* data() noexcept {
        Nullable& m = *value_;
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

    constexpr const T* data() const noexcept {
        const Nullable& m = *value_;
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

    friend constexpr auto operator<=>(const nullable_view& l,
                                      const nullable_view& r) {
        const Nullable& lhs = *l.value_;
        const Nullable& rhs = *r.value_;
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return (bool(lhs.get()) && bool(rhs.get()))
                       ? (*(lhs.get()) <=> *(rhs.get()))
                       : (bool(lhs.get()) <=> bool(rhs.get()));
        } else {
            return (bool(lhs) && bool(rhs)) ? (*lhs <=> *rhs)
                                            : (bool(lhs) <=> bool(rhs));
        }
    }

    friend constexpr bool operator==(const nullable_view& l,
                                     const nullable_view& r) {
        const Nullable& lhs = *l.value_;
        const Nullable& rhs = *r.value_;

        if constexpr (is_reference_wrapper_v<Nullable>) {
            return (bool(lhs.get()) && bool(rhs.get()))
                       ? (*(lhs.get()) == *(rhs.get()))
                       : (bool(lhs.get()) == bool(rhs.get()));
        } else {
            return (bool(lhs) && bool(rhs)) ? (*lhs == *rhs)
                                            : (bool(lhs) == bool(rhs));
        }
    }
    template <typename F>
    constexpr auto and_then(F&& f) &;
    template <typename F>
    constexpr auto and_then(F&& f) &&;
    template <typename F>
    constexpr auto and_then(F&& f) const&;
    template <typename F>
    constexpr auto and_then(F&& f) const&&;

    template <typename F>
    constexpr auto transform(F&& f) &;
    template <typename F>
    constexpr auto transform(F&& f) &&;
    template <typename F>
    constexpr auto transform(F&& f) const&;
    template <typename F>
    constexpr auto transform(F&& f) const&&;

    template <typename F>
    constexpr auto or_else(F&& f) &&;
    template <typename F>
    constexpr auto or_else(F&& f) const&;
};

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::and_then(F&& f) & {
    using U = std::invoke_result_t<F, T&>;
    static_assert(is_nullable_view_v<std::remove_cvref_t<U>>);
    if (*value_) {
        return std::invoke(std::forward<F>(f), **value_);
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::and_then(F&& f) && {
    using U = std::invoke_result_t<F, T&&>;
    static_assert(is_nullable_view_v<std::remove_cvref_t<U>>);
    if (*value_) {
        return std::invoke(std::forward<F>(f), std::move(**value_));
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::and_then(F&& f) const& {
    using U = std::invoke_result_t<F, const T&>;
    static_assert(is_nullable_view_v<std::remove_cvref_t<U>>);
    if (*value_) {
        return std::invoke(std::forward<F>(f), **value_);
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::and_then(F&& f) const&& {
    using U = std::invoke_result_t<F, const T&&>;
    static_assert(is_nullable_view_v<std::remove_cvref_t<U>>);
    if (*value_) {
        return std::invoke(std::forward<F>(f), std::move(**value_));
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::transform(F&& f) & {
    using U = std::remove_cvref_t<Nullable>;
    return (*value_) ? nullable_view<U>{std::invoke(std::forward<F>(f),
                                                           **value_)}
                     : nullable_view<U>{};
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::transform(F&& f) && {
    using U = std::remove_cvref_t<Nullable>;
    return (*value_) ? nullable_view<U>{std::invoke(std::forward<F>(f),
                                                std::move(**value_))}
                    : nullable_view<U>{};
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::transform(F&& f) const& {
    using U = std::remove_cvref_t<Nullable>;
    return (*value_)
               ? nullable_view<U>{std::invoke(std::forward<F>(f), **value_)}
               : nullable_view<U>{};
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::transform(F&& f) const&& {
    using U = std::remove_cvref_t<Nullable>;
    return (*value_) ? nullable_view<U>{std::invoke(std::forward<F>(f),
                                                   std::move(**value_))}
                    : nullable_view<U>{};
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::or_else(F&& f) const& {
    using U = std::invoke_result_t<F>;
    static_assert(std::is_same_v<std::remove_cvref_t<U>, nullable_view>);
    return *value_ ? *this : std::forward<F>(f)();
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
template <typename F>
constexpr auto nullable_view<Nullable>::or_else(F&& f) && {
    using U = std::invoke_result_t<F>;
    static_assert(std::is_same_v<std::remove_cvref_t<U>, nullable_view>);
    return *value_ ? std::move(*this) : std::forward<F>(f)();
}

template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
class nullable_view<Nullable&>
    : public ranges::view_interface<nullable_view<Nullable>> {
  private:
    using T = std::remove_reference_t<
        std::iter_reference_t<typename std::unwrap_reference_t<Nullable>>>;

    Nullable* value_;

  public:
    constexpr nullable_view() : value_(nullptr) {};

    constexpr explicit nullable_view(Nullable& nullable)
        : value_(std::addressof(nullable)) {}

    constexpr explicit nullable_view(Nullable&& nullable) = delete;

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept { return data() + size(); }
    constexpr const T* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept {
        if (!value_)
            return 0;
        Nullable& m = *value_;
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return bool(m.get());
        } else {
            return bool(m);
        }
    }

    constexpr T* data() noexcept {
        if (!value_)
            return nullptr;
        Nullable& m = *value_;
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

    constexpr const T* data() const noexcept {
        if (!value_)
            return nullptr;
        const Nullable& m = *value_;
        if constexpr (is_reference_wrapper_v<Nullable>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }
};

template <typename T>
nullable_view(T) -> nullable_view<std::decay_t<T>>;

namespace std::ranges {
template <typename T>
constexpr inline bool enable_borrowed_range<nullable_view<T*>> = true;

template <typename T>
constexpr inline bool
    enable_borrowed_range<nullable_view<std::reference_wrapper<T>>> = true;

template <typename T>
constexpr inline bool enable_borrowed_range<nullable_view<T&>> = true;

} // namespace std::ranges

namespace views {
struct __nullable_fn {
    template <typename T>
    constexpr auto operator()(T&& t) const noexcept {
        return nullable_view<std::decay_t<T>>(std::forward<T>(t));
    }
};

inline constexpr __nullable_fn nullable{};
} // namespace views

#endif
