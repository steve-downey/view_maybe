// smd/maybe.h                                               -*-C++-*-
#ifndef INCLUDED_SMD_MAYBE
#define INCLUDED_SMD_MAYBE

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

#include <optional>
#include <iterator>

namespace smd::detail {

#ifdef __cpp_lib_reference_from_temporary
using std::reference_constructs_from_temporary_v;
using std::reference_converts_from_temporary_v;
#else
template <class To, class From>
concept reference_converts_from_temporary_v =
    std::is_reference_v<To> &&
    (
        // A prvalue of a type similar to To, so that we're binding directly to
        // the materialized prvalue of type From
        (!std::is_reference_v<From> &&
         std::is_convertible_v<std::remove_cvref_t<From>*,
                               std::remove_cvref_t<To>*>) ||
        // A value of an unrelated type, convertible to To, but only by
        // materializing a To and binding a const reference; if we were trying
        // to bind a non-const reference, we'd be unable to. (This is not quite
        // exhaustive of the problem cases, but I think it's fairly close in
        // practice.)
        (std::is_lvalue_reference_v<To> &&
         std::is_const_v<std::remove_reference_t<To>> &&
         std::is_convertible_v<From, const std::remove_cvref_t<To>&&> &&
         !std::is_convertible_v<From, std::remove_cvref_t<To>&>));

template <class To, class From>
concept reference_constructs_from_temporary_v =
    // This is close in practice, because cases where conversion and
    // construction differ in semantics are rare.
    reference_converts_from_temporary_v<To, From>;
#endif

} // namespace smd::detail

namespace smd {
template <class T>
concept maybe = requires(const T t) {
    bool(t);
    *(t);
};

template <class T, class R>
constexpr auto yield_if(bool b, T&& t) -> R;

template <class R = void, maybe T, class U>
constexpr auto reference_or(T&& m, U&& u) -> decltype(auto);

template <class R = void, maybe T, class... U>
constexpr auto value_or(T&& m, U&&... u) -> decltype(auto);

template <class R = void, maybe T, class I>
constexpr auto or_invoke(T&& m, I&& invocable) -> decltype(auto);

} // namespace smd

template <class T, class R = std::optional<std::decay_t<T>>>
constexpr auto smd::yield_if(bool b, T&& t) -> R {
    return b ? std::forward<T>(t) : R{};
}

template <class R, smd::maybe T, class U>
constexpr auto smd::reference_or(T&& m, U&& u) -> decltype(auto) {
    // Find the type returned by dereferencing `m`
    using DerefType = decltype(*forward<T>(m)); // Often a reference type.

    using RetCalc = std::common_reference<DerefType, U&&>;

    // If `R` is non-void, the return type is exactly `R`, otherwise it is
    // `RetCalc::type`.
    static_assert(
        !std::is_same_v<R, void> || requires { typename RetCalc::type; },
        "No common type between value type and argument type");
    using Ret = typename std::conditional_t<std::is_same_v<R, void>,
                                            RetCalc,
                                            std::type_identity<R>>::type;

    // Check the mandates
    static_assert(std::is_constructible_v<Ret, DerefType>,
                  "Cannot construct return type from value type");
    static_assert(std::is_constructible_v<Ret, U>,
                  "Cannot construct return type from argument types");

    static_assert(
        !detail::reference_constructs_from_temporary_v<Ret, DerefType>,
        "Would construct a dangling reference from a temporary");
    static_assert(!detail::reference_constructs_from_temporary_v<Ret, U>,
                  "Would construct a dangling reference from a temporary");

    return bool(m) ? static_cast<Ret>(*std::forward<T>(m))
                   : static_cast<Ret>((U&&)u);
}

namespace smd::detail {
// Get first type in a pack containing only one type.
template <class A0>
struct pack0 {
    using type = A0;
};
template <class... Args>
using pack0_t = typename pack0<Args...>::type;
} // namespace smd::detail

template <class R, smd::maybe T, class... U>
constexpr auto smd::value_or(T&& m, U&&... u) -> decltype(auto) {
    // Construct the return value from either `*m` or `forward<U>(u)... )`.

    // Find the type returned by dereferencing `m`
    using DerefType = decltype(*forward<T>(m)); // Often a reference type.
    using ValueType = std::remove_cvref_t<DerefType>; // Non-reference value

    // If `U...` represents exactly one argument type, then `RetCalc::type` is
    // the common type of `ValueType` and `U`; otherwise `RetCalc::type` is
    // `ValueType`.  The result of this alias is a struct having a `type`
    // member, which is not "evaluated" unless needed.  If `R` is explicitly
    // non-`void`, `RetCalc::type` is never evaluated, so no error is reported
    // in cases where `U...` has length 1 but `common_type` fails to produce a
    // type.
    using RetCalc = std::conditional_t<
        sizeof...(U) == 1,
        std::common_type<ValueType, std::remove_cvref_t<U&&>...>,
        std::type_identity<ValueType>>;

    // If `R` is non-void, the return type is exactly `R`, otherwise it is
    // `RetCalc::type`.
    static_assert(
        !std::is_same_v<R, void> || requires { typename RetCalc::type; },
        "No common type between value type and argument type");
    using Ret = typename std::conditional_t<std::is_same_v<R, void>,
                                            RetCalc,
                                            std::type_identity<R>>::type;

    // Check the mandates
    static_assert(std::is_constructible_v<Ret, DerefType>,
                  "Cannot construct return type from value type");
    static_assert(std::is_constructible_v<Ret, U...>,
                  "Cannot construct return type from argument types");
    static_assert(!std::is_reference_v<Ret> || 1 == sizeof...(U),
                  "Reference return type requires exactly one argument");

    if constexpr (std::is_lvalue_reference_v<Ret> && 1 == sizeof...(U)) {
        using U0 = detail::pack0_t<U...>;
        static_assert(
            !detail::reference_constructs_from_temporary_v<Ret, DerefType>,
            "Would construct a dangling reference from a temporary");
        static_assert(!detail::reference_constructs_from_temporary_v<Ret, U0>,
                      "Would construct a dangling reference from a temporary");
    }

    return bool(m) ? static_cast<Ret>(*std::forward<T>(m))
                   : Ret(std::forward<U>(u)...);
}

template <class R = void, smd::maybe T, class IL, class... U>
constexpr auto value_or(T&& m, std::initializer_list<IL> il, U&&... u)
    -> decltype(auto) {
    // Find the type returned by dereferencing `m`
    using DerefType = decltype(*forward<T>(m)); // Often a reference type.
    using ValueType = std::remove_cvref_t<DerefType>; // Non-reference value

    using Ret = std::conditional_t<std::is_same_v<R, void>, ValueType, R>;

    // Check the mandates
    static_assert(std::is_constructible_v<Ret, DerefType>,
                  "Cannot construct return type from value type");
    static_assert(
        std::is_constructible_v<Ret, std::initializer_list<IL>, U...>,
        "Cannot construct return type from argument types");

    return bool(m) ? static_cast<Ret>(*m) : Ret(il, std::forward<U>(u)...);
}

template <class R, smd::maybe T, class I>
constexpr auto smd::or_invoke(T&& m, I&& invocable) -> decltype(auto) {
    using DerefType  = decltype(*forward<T>(m)); // Often a reference type.
    using InvokeType = std::invoke_result_t<I>;
    using CommonType = std::common_type<DerefType, InvokeType>;

    static_assert(
        !std::is_same_v<R, void> || requires { typename CommonType::type; },
        "No common type between value type and invoke result type");

    using Ret = std::conditional_t<
        std::is_same_v<R, void>,
        typename CommonType::type,
        R>;

    static_assert(std::is_constructible_v<Ret, DerefType>,
                  "Cannot construct return type from value type");
    static_assert(std::is_constructible_v<Ret, InvokeType>,
                  "Cannot construct return type from invoke result type");

    static_assert(
        !detail::reference_constructs_from_temporary_v<Ret, DerefType>,
        "Would construct a dangling reference from a temporary");
    static_assert(
        !detail::reference_constructs_from_temporary_v<Ret, InvokeType>,
        "Would construct a dangling reference from a temporary");

    return bool(m) ? static_cast<Ret>(*forward<T>(m))
                   : static_cast<Ret>(std::forward<I>(invocable)());
}
#endif
