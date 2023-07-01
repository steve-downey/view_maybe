// movable_box.h -*-C++-*-
#ifndef INCLUDED_MOVABLE_BOX
#define INCLUDED_MOVABLE_BOX

#include <concepts>
#include <optional>
#include <memory>

namespace smd::views {
namespace detail {
template <typename T>
concept boxable = std::move_constructible<T> && std::is_object_v<T>;

template <boxable T>
class movable_box : std::optional<T> {
  public:
    using std::optional<T>::optional;

    constexpr movable_box() noexcept(
        std::is_nothrow_default_constructible_v<T>)
        requires std::default_initializable<T>
        : std::optional<T>{std::in_place} {}

    movable_box(const movable_box&) = default;
    movable_box(movable_box&&)      = default;

    using std::optional<T>::operator=;

    constexpr movable_box& operator=(const movable_box& other) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        requires(!std::copyable<T> && std::copy_constructible<T>)
    {
        if (this != std::addressof(other)) {
            if (other)
                this->emplace(*other);
            else
                this->reset();
        }
        return *this;
    }
    constexpr movable_box& operator=(movable_box&& other) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        requires(!std::movable<T> && std::copy_constructible<T>)
    {
        if (this != std::addressof(other)) {
            if (other)
                this->emplace(std::move(*other));
            else
                this->reset();
        }
        return *this;
    }
};

template <boxable T>
    requires std::copyable<T> ||
             (std::move_constructible<T> &&
              std::is_nothrow_move_constructible_v<T> &&
              std::is_nothrow_copy_constructible_v<T>) ||
             (!std::copy_constructible<T> &&
              (std::copyable<T> || std::is_nothrow_move_constructible_v<T>))
class movable_box<T> {
    [[no_unique_address]] T value_ = T();

  public:
    movable_box()
        requires std::default_initializable<T>
    = default;

    constexpr explicit movable_box(const T& t) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        : value_(t) {}

    constexpr explicit movable_box(T&& t) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(t)) {}

    template <typename... _Args>
        requires std::constructible_from<T, _Args...>
    constexpr explicit movable_box(std::in_place_t, _Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, _Args...>)
        : value_(std::forward<_Args>(args)...) {}

    movable_box(const movable_box&) = default;
    movable_box(movable_box&&)      = default;
    movable_box& operator=(const movable_box&)
        requires std::copyable<T>
    = default;
    movable_box& operator=(movable_box&&)
        requires std::copyable<T>
    = default;

    constexpr movable_box& operator=(const movable_box& other) noexcept {
        static_assert(std::is_nothrow_copy_constructible_v<T>);
        if (this != std::addressof(other)) {
            value_.~T();
            std::construct_at(std::addressof(value_), *other);
        }
        return *this;
    }

    constexpr movable_box& operator=(movable_box&& other) noexcept {
        static_assert(std::is_nothrow_move_constructible_v<T>);
        if (this != std::addressof(other)) {
            value_.~T();
            std::construct_at(std::addressof(value_), std::move(*other));
        }
        return *this;
    }

    constexpr bool has_value() const noexcept { return true; };
    constexpr explicit operator bool() const noexcept { return true; };
    constexpr T&       operator*() noexcept { return value_; }
    constexpr const T& operator*() const noexcept { return value_; }
    constexpr T*       operator->() noexcept { return std::addressof(value_); }
    constexpr const T* operator->() const noexcept {
        return std::addressof(value_);
    }

    friend constexpr auto operator<=>(const movable_box& lhs,
                                      const movable_box& rhs) {
        return lhs.value_ <=> rhs.value_;
    }

    friend constexpr auto operator==(const movable_box& lhs,
                                     const movable_box& rhs) {
        return lhs.value_ == rhs.value_;
    }
};
} // namespace detail
} // namespace smd::views
#endif
