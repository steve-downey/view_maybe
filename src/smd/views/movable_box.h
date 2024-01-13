// smd/views/movable_box.h                                            -*-C++-*-
#ifndef INCLUDED_SMD_VIEWS_MOVABLE_BOX
#define INCLUDED_SMD_VIEWS_MOVABLE_BOX

#include <concepts>
#include <optional>
#include <memory>

namespace smd::views {
namespace detail {
template <typename T>
concept boxable = std::move_constructible<T> && std::is_object_v<T>;

template <boxable T>
struct movable_box : std::optional<T> {
    using std::optional<T>::optional;

    constexpr movable_box() noexcept(
        std::is_nothrow_default_constructible_v<T>)
        requires std::default_initializable<T>
        : std::optional<T>{std::in_place} {}

    movable_box(const movable_box&) = default;
    movable_box(movable_box&&)      = default;

    using std::optional<T>::operator=;

    constexpr movable_box& operator=(const movable_box& rhs) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        requires(!std::copyable<T>) && std::copy_constructible<T>
    {
        if (this != std::addressof(rhs)) {
            if ((bool)rhs)
                this->emplace(*rhs);
            else
                this->reset();
        }
        return *this;
    }

    constexpr movable_box& operator=(movable_box&& rhs) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        requires(!std::movable<T>)
    {
        if (this != std::addressof(rhs)) {
            if ((bool)rhs)
                this->emplace(std::move(*rhs));
            else
                this->reset();
        }
        return *this;
    }
};

template <typename T>
concept boxable_copyable =
    std::copy_constructible<T> &&
    (std::copyable<T> || (std::is_nothrow_move_constructible_v<T> &&
                          std::is_nothrow_copy_constructible_v<T>));
template <typename T>
concept boxable_movable = (!std::copy_constructible<T>)&&(
    std::movable<T> || std::is_nothrow_move_constructible_v<T>);

template <boxable T>
    requires boxable_copyable<T> || boxable_movable<T>
struct movable_box<T> {
  private:
    [[no_unique_address]] T value_ = T();

  public:
    movable_box()
        requires std::default_initializable<T>
    = default;

    constexpr explicit movable_box(const T& t) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        requires std::copy_constructible<T>
        : value_(t) {}

    constexpr explicit movable_box(T&& t) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(t)) {}

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    constexpr explicit movable_box(std::in_place_t, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : value_(std::forward<Args>(args)...) {}

    movable_box(const movable_box&) = default;
    movable_box(movable_box&&)      = default;
    movable_box& operator=(const movable_box&)
        requires std::copyable<T>
    = default;
    movable_box& operator=(movable_box&&)
        requires std::movable<T>
    = default;

    constexpr movable_box& operator=(const movable_box& rhs) noexcept
        requires(!std::copyable<T>) && std::copy_constructible<T>
    {
        static_assert(std::is_nothrow_copy_constructible_v<T>);
        if (this != std::addressof(rhs)) {
            value_.~T();
            std::construct_at(std::addressof(value_), *rhs);
        }
        return *this;
    }

    constexpr movable_box& operator=(movable_box&& rhs) noexcept
        requires(!std::movable<T>)
    {
        static_assert(std::is_nothrow_move_constructible_v<T>);
        if (this != std::addressof(rhs)) {
            value_.~T();
            std::construct_at(std::addressof(value_), std::move(*rhs));
        }
        return *this;
    }

    constexpr bool has_value() const noexcept { return true; };

    constexpr T& operator*() & noexcept { return value_; }

    constexpr const T& operator*() const& noexcept { return value_; }

    constexpr T&& operator*() && noexcept { return std::move(value_); }

    constexpr const T&& operator*() const&& noexcept {
        return std::move(value_);
    }

    constexpr T* operator->() noexcept { return std::addressof(value_); }

    constexpr const T* operator->() const noexcept {
        return std::addressof(value_);
    }

    constexpr operator bool() const noexcept { return true; };
};

} // namespace detail
} // namespace smd::views
#endif
