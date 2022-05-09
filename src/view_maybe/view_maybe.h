// view_maybe.h                                                       -*-C++-*-
#ifndef INCLUDED_VIEW_MAYBE
#define INCLUDED_VIEW_MAYBE

#include <concepts>
#include <ranges>
#include <iostream>

namespace ranges = std::ranges;

template <class Ref, class ConstRef>
concept readable_references =
    std::is_lvalue_reference_v<Ref>&&   std::is_object_v<
        std::remove_reference_t<Ref>>&& std::is_lvalue_reference_v<ConstRef>&&
                                        std::is_object_v<std::remove_reference_t<ConstRef>>&&
                                        std::convertible_to<std::add_pointer_t<ConstRef>,
                                const std::remove_reference_t<Ref>*>;

template <class T>
concept nullable = std::is_object_v<T>&& requires(T& t, const T& ct) {
    bool(ct);
    *(t);
    *(ct);
};

template <class T>
concept nullable_val =
    nullable<T>&& readable_references<std::iter_reference_t<T>,
                                      std::iter_reference_t<const T>>;

template <typename, template <typename...> class>
inline constexpr bool is_v = false;
template <typename... Ts, template <typename...> class C>
inline constexpr bool is_v<C<Ts...>, C> = true;

template <class T>
concept nullable_ref =
    is_v<T, std::reference_wrapper>&& nullable_val<typename T::type>;

template <class T>
inline constexpr bool is_reference_wrapper_v = is_v<T, std::reference_wrapper>;

template <class T>
concept copyable_object = (std::copy_constructible<T> && std::is_object_v<T>);

template <typename Value>
requires(copyable_object<Value>) class maybe_view
    : public ranges::view_interface<maybe_view<Value>> {
  private:
    std::optional<Value> value_;

  public:
    constexpr maybe_view() = default;

    constexpr explicit maybe_view(Value const& value) : value_(value) {}

    constexpr explicit maybe_view(Value&& value) : value_(std::move(value)) {}

    template <class... Args>
    requires std::constructible_from<Value, Args...> constexpr maybe_view(
        std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...) {}

    constexpr Value*       begin() noexcept { return data(); }
    constexpr const Value* begin() const noexcept { return data(); }
    constexpr Value*       end() noexcept { return data() + size(); }
    constexpr const Value* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept { return bool(value_); }

    constexpr Value* data() noexcept { return std::addressof(*value_); }

    constexpr const Value* data() const noexcept {
        return std::addressof(*value_);
    }
};

template <typename Maybe>
requires(copyable_object<Maybe> &&
         (nullable_val<Maybe> || nullable_ref<Maybe>))
class maybe_view<Maybe>
    : public ranges::view_interface<maybe_view<Maybe>> {
  private:
    using T = std::remove_reference_t<
        std::iter_reference_t<typename std::unwrap_reference_t<Maybe>>>;

    ranges::__detail::__box<Maybe> value_;

  public:
    constexpr maybe_view() = default;

    constexpr explicit maybe_view(Maybe const& maybe) : value_(maybe) {}

    constexpr explicit maybe_view(Maybe&& maybe) : value_(std::move(maybe)) {}

    template <class... Args>
    requires std::constructible_from<Maybe, Args...> constexpr maybe_view(
        std::in_place_t, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...) {}

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept { return data() + size(); }
    constexpr const T* end() const noexcept { return data() + size(); }

    constexpr size_t size() const noexcept {
        if constexpr (is_reference_wrapper_v<Maybe>) {
            return bool((*value_).get());
        } else {
            return bool(*value_);
        }
    }

    constexpr T* data() noexcept {
        Maybe& m = *value_;
        if constexpr (is_reference_wrapper_v<Maybe>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
    }

    constexpr const T* data() const noexcept {
        const Maybe& m = *value_;
        if constexpr (is_reference_wrapper_v<Maybe>) {
            return m.get() ? std::addressof(*(m.get())) : nullptr;
        } else {
            return m ? std::addressof(*m) : nullptr;
        }
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
