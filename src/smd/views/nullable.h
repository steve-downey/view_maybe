// smd/views/nullable.h                                               -*-C++-*-
#ifndef INCLUDED_SMD_VIEWS_NULLABLE
#define INCLUDED_SMD_VIEWS_NULLABLE
#include <concepts>
#include <ranges>
#include <type_traits>
#include <smd/views/concepts.h>
#include <smd/views/movable_box.h>

namespace smd::views {

namespace ranges = std::ranges;

template <typename T>
class nullable_view;

template <typename Nullable>
    requires(movable_object<Nullable> && nullable_object<Nullable>)
class nullable_view<Nullable>
    : public ranges::view_interface<nullable_view<Nullable>> {
  private:
    using U = std::remove_reference_t<
        std::iter_reference_t<typename std::unwrap_reference_t<Nullable>>>;

    detail::movable_box<Nullable> value_;

  public:
    constexpr nullable_view() = default;

    constexpr explicit nullable_view(const Nullable& nullable)
        : value_(nullable) {}

    constexpr explicit nullable_view(Nullable&& nullable)
        : value_(std::move(nullable)) {}

    template <class... Args>
        requires std::constructible_from<Nullable, Args...>
    constexpr nullable_view(std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...) {}

    constexpr U*       begin() noexcept { return data(); }
    constexpr const U* begin() const noexcept { return data(); }
    constexpr U*       end() noexcept { return data() + size(); }
    constexpr const U* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept {
        const Nullable& m = *value_;
        return bool(m);
    }

    constexpr U* data() noexcept {
        Nullable& m = *value_;
        return m ? std::addressof(*m) : nullptr;
    }

    constexpr const U* data() const noexcept {
        const Nullable& m = *value_;
        return m ? std::addressof(*m) : nullptr;
    }

    friend constexpr auto operator<=>(const nullable_view& l,
                                      const nullable_view& r) {
        const Nullable& lhs = *l.value_;
        const Nullable& rhs = *r.value_;
        return (bool(lhs) && bool(rhs)) ? (*lhs <=> *rhs)
                                        : (bool(lhs) <=> bool(rhs));
    }

    friend constexpr bool operator==(const nullable_view& l,
                                     const nullable_view& r) {
        const Nullable& lhs = *l.value_;
        const Nullable& rhs = *r.value_;
        return (bool(lhs) && bool(rhs)) ? (*lhs == *rhs)
                                        : (bool(lhs) == bool(rhs));
    }
};

template <typename Nullable>
    requires(nullable_object<Nullable>)
class nullable_view<Nullable&>
    : public ranges::view_interface<nullable_view<Nullable>> {
  private:
    using U = std::remove_reference_t<
        std::iter_reference_t<typename std::unwrap_reference_t<Nullable>>>;

    Nullable* value_;

  public:
    constexpr nullable_view() : value_(nullptr) {};

    constexpr explicit nullable_view(Nullable& nullable)
        : value_(std::addressof(nullable)) {}

    constexpr explicit nullable_view(Nullable&& nullable) = delete;

    constexpr U*       begin() noexcept { return data(); }
    constexpr const U* begin() const noexcept { return data(); }
    constexpr U*       end() noexcept { return data() + size(); }
    constexpr const U* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept {
        if (!value_)
            return 0;
        return bool(*value_);
    }

    constexpr U* data() noexcept {
        if (!value_)
            return nullptr;
        return *value_ ? std::addressof(**value_) : nullptr;
    }

    constexpr const U* data() const noexcept {
        if (!value_)
            return nullptr;
        return *value_ ? std::addressof(**value_) : nullptr;
    }

    friend constexpr auto operator<=>(const nullable_view& l,
                                      const nullable_view& r) {
        return (bool(l.value_) && bool(r.value_))
                   ? (*l.value_ <=> *r.value_)
                   : (bool(l.value_) <=> bool(r.value_));
    }
    friend constexpr bool operator==(const nullable_view& l,
                                     const nullable_view& r) {
        return (bool(l.value_) && bool(r.value_))
                   ? (*l.value_ == *r.value_)
                   : (bool(l.value_) == bool(r.value_));
    }
};

template <typename T>
nullable_view(T) -> nullable_view<std::decay_t<T>>;

} // namespace smd::views

namespace std::ranges {
template <typename T>
inline constexpr bool enable_borrowed_range<smd::views::nullable_view<T*>> =
    true;

template <typename T>
inline constexpr bool enable_borrowed_range<smd::views::nullable_view<T&>> =
    true;
} // namespace std::ranges

namespace smd::views {
struct __nullable_fn {
    template <typename T>
    constexpr auto operator()(T&& t) const noexcept {
        return nullable_view<std::decay_t<T>>(std::forward<T>(t));
    }
};

inline constexpr __nullable_fn nullable{};
} // namespace smd::views

#endif
