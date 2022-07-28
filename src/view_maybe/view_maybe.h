// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

#include <concepts>
#include <ranges>
#include <iostream>
#include <type_traits>
#include <view_maybe/concepts.h>

namespace ranges = std::ranges;

template <typename Value>
class maybe_view : public ranges::view_interface<maybe_view<Value>> {
  private:
    std::optional<Value> value_;

  public:
    constexpr maybe_view() = default;

    constexpr explicit maybe_view(Value const& value) : value_(value) {}

    constexpr explicit maybe_view(Value&& value) : value_(std::move(value)) {}

    template <class... Args>
        requires std::constructible_from<Value, Args...>
    constexpr maybe_view(std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...) {}

    constexpr Value*       begin() noexcept { return data(); }
    constexpr const Value* begin() const noexcept { return data(); }
    constexpr Value*       end() noexcept { return data() + size(); }
    constexpr const Value* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept { return bool(value_); }

    constexpr Value* data() noexcept {
        return value_ ? std::addressof(*value_) : nullptr;
    }

    constexpr const Value* data() const noexcept {
        return value_ ? std::addressof(*value_) : nullptr;
    }

    friend constexpr auto operator<=>(const maybe_view& lhs,
                                      const maybe_view& rhs) {
        return lhs.value_ <=> rhs.value_;
    }

    friend constexpr bool operator==(const maybe_view& lhs,
                                     const maybe_view& rhs) {
        return lhs.value_ == rhs.value_;
    }
};

template <typename Value>
class maybe_view<Value&> : public ranges::view_interface<maybe_view<Value&>> {
  private:
    Value* value_;

  public:
    constexpr maybe_view() : value_(nullptr) {}

    constexpr explicit maybe_view(Value& value)
        : value_(std::addressof(value)) {}

    constexpr explicit maybe_view(Value&& value) = delete;

    template <class... Args>
        requires std::constructible_from<Value, Args...>
    constexpr maybe_view(std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...) {}

    constexpr Value*       begin() noexcept { return data(); }
    constexpr const Value* begin() const noexcept { return data(); }
    constexpr Value*       end() noexcept { return data() + size(); }
    constexpr const Value* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept { return bool(value_); }

    constexpr Value* data() noexcept {
        if (!value_)
            return nullptr;
        Value& m = *value_;
        return value_ ? std::addressof(m) : nullptr;
    }

    constexpr const Value* data() const noexcept {
        if (!value_)
            return nullptr;
        const Value& m = *value_;
        return value_ ? std::addressof(m) : nullptr;
    }

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
};

namespace std::ranges {
template <typename T>
constexpr inline bool enable_borrowed_range<maybe_view<T*>> = true;

template <typename T>
constexpr inline bool
    enable_borrowed_range<maybe_view<std::reference_wrapper<T>>> = true;
} // namespace std::ranges

namespace views {
struct __maybe_fn {
    template <typename T>
    constexpr auto operator()(T&& t) const noexcept {
        return maybe_view{t};
    }
};

inline constexpr __maybe_fn maybe{};
} // namespace views

#endif
