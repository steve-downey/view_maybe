// smd/views/maybe.h                                                  -*-C++-*-
#ifndef INCLUDED_SMD_VIEWS_MAYBE
#define INCLUDED_SMD_VIEWS_MAYBE

#include <concepts>
#include <functional>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <utility>

#include <smd/views/concepts.h>

namespace smd::views {
namespace ranges = std::ranges;

#if __cpp_lib_forward_like >= 202207L
template <class T, class U>
[[nodiscard]] constexpr auto&& forward_like(U&& x) noexcept {
    constexpr bool is_adding_const =
        std::is_const_v<std::remove_reference_t<T>>;
    if constexpr (std::is_lvalue_reference_v<T&&>) {
        if constexpr (is_adding_const)
            return std::as_const(x);
        else
            return static_cast<U&>(x);
    } else {
        if constexpr (is_adding_const)
            return std::move(std::as_const(x));
        else
            return std::move(x);
    }
}
#else
using std::forward_like;
#endif

template <typename Value>
class maybe_view;

template <typename Value>
inline constexpr bool is_maybe_view_v = false;
template <typename Value>
inline constexpr bool is_maybe_view_v<maybe_view<Value>> = true;

template <typename Value>
class maybe_view : public ranges::view_interface<maybe_view<Value>> {
  private:
    [[no_unique_address]] std::optional<Value> value_;

  public:
    constexpr maybe_view() = default;

    constexpr explicit maybe_view(const Value& value);

    constexpr explicit maybe_view(Value&& value);

    template <class... Args>
        requires std::constructible_from<Value, Args...>
    constexpr maybe_view(std::in_place_t, Args&&... args);

    constexpr Value*       begin() noexcept;
    constexpr const Value* begin() const noexcept;
    constexpr Value*       end() noexcept;
    constexpr const Value* end() const noexcept;

    constexpr size_t size() const noexcept;

    constexpr Value* data() noexcept;

    constexpr const Value* data() const noexcept;
    friend constexpr auto  operator<=>(const maybe_view& lhs,
                                      const maybe_view& rhs) {
        {
            return (lhs.value_ && rhs.value_)
                        ? (*lhs.value_ <=> *rhs.value_)
                        : (bool(lhs.value_) <=> bool(rhs.value_));
        }
    }

    friend constexpr bool operator==(const maybe_view& lhs,
                                     const maybe_view& rhs) {
        return lhs.value_ == rhs.value_;
    }

    template <typename Self, typename F>
    constexpr auto and_then(this Self&& self, F&& f);

    template <typename Self, typename F>
    constexpr auto transform(this Self&& self, F&& f);

    template <typename Self, typename F>
    constexpr auto or_else(this Self&& self, F&& f);
};

template <typename Value>
constexpr maybe_view<Value>::maybe_view(const Value& value) : value_(value) {}

template <typename Value>
constexpr maybe_view<Value>::maybe_view(Value&& value)
    : value_(std::move(value)) {}

template <typename Value>
template <class... Args>
    requires std::constructible_from<Value, Args...>
constexpr maybe_view<Value>::maybe_view(std::in_place_t, Args&&... args)
    : value_(std::in_place, std::forward<Args>(args)...) {}

template <typename Value>
constexpr Value* maybe_view<Value>::begin() noexcept {
    return data();
}
template <typename Value>
constexpr const Value* maybe_view<Value>::begin() const noexcept {
    return data();
}
template <typename Value>
constexpr Value* maybe_view<Value>::end() noexcept {
    return data() + size();
}
template <typename Value>
constexpr const Value* maybe_view<Value>::end() const noexcept {
    return data() + size();
}

template <typename Value>
constexpr size_t maybe_view<Value>::size() const noexcept {
    return bool(value_);
}

template <typename Value>
constexpr Value* maybe_view<Value>::data() noexcept {
    return value_ ? std::addressof(*value_) : nullptr;
}

template <typename Value>
constexpr const Value* maybe_view<Value>::data() const noexcept {
    return value_ ? std::addressof(*value_) : nullptr;
}

template <typename Value>
template <typename Self, typename F>
constexpr auto maybe_view<Value>::and_then(this Self&& self, F&& f) {
    using U =
        std::invoke_result_t<F, decltype(forward_like<Self>(*self.value_))>;
    static_assert(is_maybe_view_v<std::remove_cvref_t<U>>);
    if (self.value_) {
        return std::invoke(std::forward<F>(f),
                           forward_like<Self>(*self.value_));
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <typename Value>
template <typename Self, typename F>
constexpr auto maybe_view<Value>::transform(this Self&& self, F&& f) {
    using U =
        std::invoke_result_t<F, decltype(forward_like<Self>(*self.value_))>;
    return (self.value_)
               ? maybe_view<U>{std::invoke(std::forward<F>(f),
                                           forward_like<Self>(*self.value_))}
               : maybe_view<U>{};
}

template <typename Value>
    template <typename Self, typename F>
    constexpr auto maybe_view<Value>::or_else(this Self&& self, F&& f) {
        using U = std::invoke_result_t<F>;
        static_assert(std::is_same_v<std::remove_cvref_t<U>, maybe_view>);
        return self.value_ ? std::forward<Self>(self) : std::forward<F>(f)();
    }

/////////////////////
// maybe_view<T&>
////////////////////

template <typename Value>
class maybe_view<Value&> : public ranges::view_interface<maybe_view<Value&>> {
  private:
    Value* value_;

  public:
    constexpr maybe_view();

    constexpr explicit maybe_view(Value& value);

    constexpr explicit maybe_view(Value&& value) = delete;

    constexpr Value*       begin() noexcept;
    constexpr const Value* begin() const noexcept;
    constexpr Value*       end() noexcept;
    constexpr const Value* end() const noexcept;

    constexpr size_t size() const noexcept;

    constexpr Value* data() noexcept;

    constexpr const Value* data() const noexcept;

    friend constexpr auto operator<=>(const maybe_view& lhs,
                                      const maybe_view& rhs) {
        return (bool(lhs.value_) && bool(rhs.value_))
                   ? (*lhs.value_ <=> *rhs.value_)
                   : (bool(lhs.value_) <=> bool(rhs.value_));
    }

    friend constexpr bool operator==(const maybe_view& lhs,
                                     const maybe_view& rhs) {
        return (bool(lhs.value_) && bool(rhs.value_))
                   ? (*lhs.value_ == *rhs.value_)
                   : (bool(lhs.value_) == bool(rhs.value_));
    }

    template <typename Self, typename F>
    constexpr auto and_then(this Self&& self, F&& f);

    template <typename Self, typename F>
    constexpr auto transform(this Self&& self, F&& f);

    template <typename Self, typename F>
    constexpr maybe_view or_else(this Self&& self, F&& f);

    // template <typename F>
    // constexpr maybe_view or_else(F&& f) &&;
    // template <typename F>
    // constexpr maybe_view or_else(F&& f) const&;
};

template <typename Value>
constexpr maybe_view<Value&>::maybe_view() : value_(nullptr) {}

template <typename Value>
constexpr maybe_view<Value&>::maybe_view(Value& value)
    : value_(std::addressof(value)) {}

template <typename Value>
constexpr Value* maybe_view<Value&>::begin() noexcept {
    return data();
}
template <typename Value>
constexpr const Value* maybe_view<Value&>::begin() const noexcept {
    return data();
}
template <typename Value>
constexpr Value* maybe_view<Value&>::end() noexcept {
    return data() + size();
}
template <typename Value>
constexpr const Value* maybe_view<Value&>::end() const noexcept {
    return data() + size();
}

template <typename Value>
constexpr size_t maybe_view<Value&>::size() const noexcept {
    return bool(value_);
}

template <typename Value>
constexpr Value* maybe_view<Value&>::data() noexcept {
    if (!value_)
        return nullptr;
    return std::addressof(*value_);
}

template <typename Value>
constexpr const Value* maybe_view<Value&>::data() const noexcept {
    if (!value_)
        return nullptr;
    return std::addressof(*value_);
}

template <typename Value>
template <typename Self, typename F>
constexpr auto maybe_view<Value&>::and_then(this Self&& self, F&& f) {
    using U =
        std::invoke_result_t<F, decltype(forward_like<Self>(*self.value_))>;
    static_assert(is_maybe_view_v<std::remove_cvref_t<U>>);
    if (self.value_) {
        return std::invoke(std::forward<F>(f),
                           forward_like<Self>(*self.value_));
    } else {
        return std::remove_cvref_t<U>();
    }
}

template <typename Value>
template <typename Self, typename F>
constexpr auto maybe_view<Value&>::transform(this Self&& self, F&& f) {
    using U =
        std::invoke_result_t<F, decltype(forward_like<Self>(*self.value_))>;
    return (self.value_)
               ? maybe_view<U>{std::invoke(std::forward<F>(f),
                                           forward_like<Self>(*self.value_))}
               : maybe_view<U>{};
}

template <typename Value>
template <typename Self, typename F>
constexpr maybe_view<Value&> maybe_view<Value&>::or_else(this Self&& self,
                                                         F&&         f) {
    using U = std::invoke_result_t<F>;
    static_assert(std::is_same_v<std::remove_cvref_t<U>, maybe_view<Value&>>);
    return self.value_ ? std::forward<Self>(self) : std::forward<F>(f)();
}

template <typename Value>
maybe_view(Value) -> maybe_view<Value>;

} // namespace smd::views

namespace std::ranges {
template <typename T>
inline constexpr bool enable_borrowed_range<smd::views::maybe_view<T*>> = true;

template <typename T>
inline constexpr bool
    enable_borrowed_range<smd::views::maybe_view<std::reference_wrapper<T>>> =
        true;

template <typename T>
inline constexpr bool enable_borrowed_range<smd::views::maybe_view<T&>> = true;

} // namespace std::ranges

namespace smd::views {
struct __maybe_fn {
    template <typename T>
    constexpr auto operator()(T&& t) const noexcept {
        return maybe_view{t};
    }
};

inline constexpr __maybe_fn maybe{};
} // namespace smd::views
#endif
