// view_nullable.h                                                    -*-C++-*-
#ifndef INCLUDED_VIEW_NULLABLE
#define INCLUDED_VIEW_NULLABLE
#include <concepts>
#include <ranges>
#include <iostream>
#include <type_traits>
#include <view_maybe/concepts.h>

namespace ranges = std::ranges;


template <typename Nullable>
    requires(copyable_object<Nullable> &&
             (nullable_object_val<Nullable> || nullable_object_ref<Nullable>))
class nullable_view : public ranges::view_interface<nullable_view<Nullable>> {
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
};

namespace std::ranges {
template <typename T>
constexpr inline bool enable_borrowed_range<nullable_view<T*>> = true;

template <typename T>
constexpr inline bool
    enable_borrowed_range<nullable_view<std::reference_wrapper<T>>> = true;
} // namespace std::ranges


namespace views {
struct __nullable_fn {
    template <typename T>
    constexpr auto operator()(T&& t) const noexcept {
        return nullable_view{t};
    }
};

inline constexpr __nullable_fn nullable{};
} // namespace views

#endif
