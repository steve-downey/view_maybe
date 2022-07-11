---
title: "A view of 0 or 1 elements: `views::maybe`"
document: D1255R8
date: today
audience: SG9, LEWG
author:
  - name: Steve Downey
    email: <sdowney2@bloomberg.net>, <sdowney@gmail.com>
toc: false
---

Abstract: This paper proposes `views::maybe` a range adaptor that produces a view with cardinality 0 or 1 which adapts copyable object types, values, and nullable types such as `std::optional` and pointer to object types.



# Changes
## Changes since R7

## Changes since R6
- Extend to all object types in order to support list comprehension
- Track working draft changes for Ranges
- Add discussion of _borrowed_range_
- Add an example where pipelines use references.
- Add support for proxy references (explore std::pointer_traits, etc).
- Make std::views::maybe model std::ranges::borrowed_range if it's not holding the object by value.
- Add a const propagation section discussing options, existing precedent and proposing the option that the author suggests.

## Changes since R5
- Fix reversed before/after table entry
- Update to match C++20 style [@N4849] and changes in Ranges since [@P0896R3]
- size is now size_t, like other ranges are also
- add synopsis for adding to `<ranges>` header
- Wording clean up, formatting, typesetting
- Add implementation notes and references

## Changes since R4

- Use std::unwrap\_reference
- Remove conditional `noexcept`ness
- Adopted the great concept renaming

## Changes since R3

- Always Capture
- Support reference\_wrapper

## Changes since R2

- Reflects current code as reviewed
- Nullable concept specification

Remove Readable as part of the specification, use the useful requirements from Readable

- Wording for views::maybe as proposed-
- Appendix A: wording for a view\_maybe that always captures

## Changes since R1

- Refer to views::all
Behavior of capture vs refer is similar to how views::all works over the expression it is given

- Use wording 'range adaptor object'
Match current working paper language


## Changes since R0


- Remove customization point objects

Removed views::maybe\_has\_value and views::maybe\_value, instead requiring that the nullable type be dereferenceable and contextually convertible to bool.


- Concept `Nullable`, for exposition

Concept Nullable, which is Readable and contextually convertible to bool


- Capture rvalues by decay copy

Hold a copy when constructing a view over a nullable rvalue.


- Remove maybe\_view as a specified type

Introduced two exposition types, one safely holding a copy, the other referring to the nullable


# Before / After Table

::: tonytable

### Before
```C++
{
    auto&& opt = possible_value();
    if (opt) {
        // a few dozen lines ...
        use(*opt); // is *opt OK ?
    }
}
```

### After
```C++

for (auto&& opt : views::maybe(possible_value())) {

    // a few dozen lines ...
    use(opt); // opt is OK
}
```

---

```C++
std::optional o{7};
if (o) {
    *o = 9;
    std::cout << "o=" << *o << " prints 9\n";
}
std::cout << "o=" << *o << " prints 9\n";

```

```C++
std::optional o{7};
for (auto&& i : views::maybe(std::ref(o))) {
    i = 9;
    std::cout << "i=" << i << " prints 9\n";
}
std::cout << "o=" << *o << " prints 9\n";
```

---

```C++
std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
auto test = [](int i) -> std::optional<int> {
    switch (i) {
      case 1:
      case 3:
      case 7:
      case 9:
    return i;
      default:
    return {};
    }
};

auto&& r = v |
    ranges::views::transform(test) |
    ranges::views::filter([](auto x){return bool(x);}) |
    ranges::views::transform([](auto x){return *x;}) |
    ranges::views::transform(
        [](int i) {
            std::cout << i;
            return i;
        });

```

```C++
std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
auto test = [](int i) -> std::optional<int> {
    switch (i) {
      case 1:
      case 3:
      case 7:
      case 9:
    return i;
      default:
    return {};
    }
};

auto&& r = v |
    ranges::views::transform(test) |
    ranges::views::transform(views::maybe) |
    ranges::views::join |
    ranges::views::transform(
        [](int i) {
            std::cout << i;
            return i;
        });
```
:::

# Motivation

In writing range transformation it is useful to be able to lift a value into a view that is either empty or contains the value. For types that are `nullable`, constructing an empty view for disengaged values and providing a view to the underlying value is useful as well. The adapter `views::single` fills a similar purpose for non-nullable values, lifting a single value into a view, and `views::empty` provides a range of no values of a given type. The type `views::maybe` can be used to unify `single` and `empty` into a single type for further processing. This is in particuluar useful when translating list comprehensions.

```C++
std::vector<std::optional<int>> v{
  std::optional<int>{42},
  std::optional<int>{},
  std::optional<int>{6 * 9}};

auto r = views::join(views::transform(v, views::maybe));

for (auto i : r) {
    std::cout << i; // prints 42 and 54
}
```

In addition to range transformation pipelines, `views::maybe` can be used in range based for loops, allowing the nullable value to not be dereferenced within the body. This is of small value in small examples in contrast to testing the nullable in an if statement, but with longer bodies the dereference is often far away from the test. Often the first line in the body of the `if` is naming the dereferenced nullable, and lifting the dereference into the for loop eliminates some boilerplate code, the same way that range based for loops do.

```C++
{
    auto&& opt = possible_value();
    if (opt) {
        // a few dozen lines ...
        use(*opt); // is *opt OK ?
    }
}

for (auto&& opt : views::maybe(possible_value())) {
    // a few dozen lines ...
    use(opt); // opt is OK
}
```

The view can be on a `std::reference_wrapper`, allowing the underlying nullable to be modified:

```C++
std::optional o{7};
for (auto&& i : views::maybe(std::ref(o))) {
    i = 9;
    std::cout << "i=" << i << " prints 9\n";
}
std::cout << "o=" << *o << " prints 9\n";
```

Of course, if the nullable is empty, there is nothing in the view to modify.

```C++
auto oe = std::optional<int>{};
for (int i : views::maybe(std::ref(oe)))
    std::cout << "i=" << i << '\n'; // does not print
```

Converting an optional type into a view can make APIs that return optional types, such a lookup operations, easier to work with in range pipelines.

```C++
std::unordered_set<int> set{1, 3, 7, 9};

auto flt = [=](int i) -> std::optional<int> {
    if (set.contains(i))
        return i;
    else
        return {};
};

for (auto i : ranges::iota_view{1, 10} | ranges::views::transform(flt)) {
    for (auto j : views::maybe(i)) {
        for (auto k : ranges::iota_view(0, j))
            std::cout << '\a';
        std::cout << '\n';
    }
}

// Produce 1 ring, 3 rings, 7 rings, and 9 rings
```

# Lazy monadic pythagorean triples
Eric Niebler's pythagorean triple example, using current C++ and proposed views::maybe.

```C++

// "and_then" creates a new view by applying a
// transformation to each element in an input
// range, and flattening the resulting range of
// ranges. A.k.a. bind
// (This uses one syntax for constrained lambdas
// in C++20.)
inline constexpr auto and_then = [](auto&& r, auto fun) {
    return decltype(r)(r) | std::ranges::views::transform(std::move(fun)) |
           std::ranges::views::join;
};

// "yield_if" takes a bool and a value and
// returns a view of zero or one elements.
inline constexpr auto yield_if = [](bool b, auto x) {
    return b ? maybe_view{std::move(x)}
             : maybe_view<decltype(x)>{};
};

void print_triples() {
    using std::ranges::views::iota;
    auto triples = and_then(iota(1), [](int z) {
        return and_then(iota(1, z + 1), [=](int x) {
            return and_then(iota(x, z + 1), [=](int y) {
                return yield_if(x * x + y * y == z * z,
                                std::make_tuple(x, y, z));
            });
        });
    });

    // Display the first 10 triples
    for (auto triple : triples | std::ranges::views::take(10)) {
        std::cout << '(' << std::get<0>(triple) << ',' << std::get<1>(triple)
                  << ',' << std::get<2>(triple) << ')' << '\n';
    }
}
```

The implementation of yield\_if is essentially the type unification of `single` and `empty` into `maybe`, returning an empty on false, and a range containing one value on true.

# Proposal

Add a range adaptor object `views::maybe`, returning a view over an object, capturing by value. Dor `nullable` objects, provide a zero size range for objects which are disengaged. A _`nullable`_ object is one that is both contextually convertible to bool and for which the type produced by dereferencing is an equality preserving object. Non void pointers, `std::optional`, and the proposed  `expected` [@P0323R9] types all models _`nullable`_. Function pointers do not, as functions are not objects. Iterators do not generally model _`nullable`_, as they are not required to be contextually convertible to bool.


# Design

The basis of the design is to hybridize `views::single` and `views::empty`. If the view is over a value that is not `nullable` it is like a single view if constructed with a value, or is of size zero otherwise. For `nullable` types, if the underlying object claims to hold a value, as determined by checking if the object when converted to bool is true, `begin` and `end` of the view are equivalent to the address of the held value within the underlying object and one past the underlying object. If the underlying object does not have a value, `begin` and `end` return `nullptr`.

# Borrowed Range
A `borrowed_range` is one whose iterators cannot be invalidated by ending the lifetime of the range. For views::maybe, the iterators are T*, where T is essentially the type of the dereferenced nullable. For raw pointers and `reference_wrapper` over nullable types, the iterator for `maybe_view` points directly to the underlying object, and thus matches the semantics of `borrowed_range`. This means that `maybe_view` is conditionally borrowed. A `maybe_view<shared_ptr>`, however, is not a borrowed range, as it participates in ownership of the shared_ptr and might invalidate the iterators if upon the end of its lifetime it is the last owner.

An example of code that is enabled by borrowed ranges, if unlikely code:
```C+++
num = 42;
int k = *std::ranges::find(views::maybe(&num), num);
```

Providing the facility is not a signficant cost, and conveys the semantics correctly, even if the simple examples are not hugely motivating. Particularly as there is no real implementation impact, other than providing template variable specializations for `enable_borrowed_range`.


# List Comprehension Desugaring
The case for having maybe_view available is seen in desugaring list comprehensions, where they naturally show up in guard clauses.

Looking at Haskell for a [formal lowering of comprehensions](https://ghc.gitlab.haskell.org/ghc/doc/users_guide/exts/monad_comprehensions.html):

We write `Desugar[ e | Q]` to mean the desugaring of the comprehension `[ e | Q]`

Expressions: `e`

Lists of qualifiers: `Q`,`R`,`S`

-- Basic forms

`Desugar[ e | ]`               = `return e`

`Desugar[ e | p <- e, Q ]`     = `e >>= \p -> Desugar[ e | Q ]`

`Desugar[ e | e, Q ]`          = `guard e >> \p -> Desugar[ e | Q ]`

Where:

`>>=` is the normal bind operator

`(>>=) :: m a -> (a -> m b) -> m b `

equivalent to

`\x -> join (fmap f x)`

See the `abd_then` function above.

`>>` is a bind that sequences but discards the left side

`(>>) :: m a -> m b -> m b`

defined as

`k >> f =
    k >>= (\_ -> f)`

`guard` has the type `guard :: Alternative f => Bool -> f ()` and is defined as

```haskell
guard True  = return ()
guard False = empty
```

See the `yield_if` above.

`return` is a constructor for a monad over T, lifting a value into the monad. `return :: Monad m => t -> m t`

The pythagorean triple example above is a typical hand desugaring of a list comprehension.
```
 [(x,y,z) | z <- [1..x], y <- [1..z], x <- [1..y], x*x + y*y== z*z]
```

The `guard` function functions as a filter. It's usually possible to rewrite the guard and join into a filter function, but fusing the guard conditions may not be straightforward.



# Implementation

A publically available implementation at <https://github.com/steve-downey/view_maybe> based on the Ranges implementation in libstdc++ . There are no particular implementation difficulties or tricks. The declarations are essentially what is quoted in the Wording section and the implementations are described as *Effects*.

[Compiler Explorer Link to Before/After Examples](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGe1wAyeAyYAHI%2BAEaYxCDSAA6oCoRODB7evnoJSY4CQSHhLFEx0naYDilCBEzEBGk%2Bfly2mPY5DJXVBHlhkdGxtlU1dRmNCgOdwd2FvZIAlLaoXsTI7BzmAMzByN5YANQma24IBARxCiAA9OfETADuAHTAhAheEV5KS7KMBHdoLOejmAAbpgALToVA3EIAT3OgLwmBuAH0WEwoVFziiAcR/ktYfCkSi0Zg8QjkaioncEPtsCYNABBWkMswbBhbLy7fZuYEOEjUxmM0boEAgVBxVpiTnBAjUnZZJIReiIwFiLyYCAzfZWemA1B4dA7N5qqU7AgatZauk6vU7KL8YiYDTq/kAdgtO3de1djI9OyYXiI5gAbEGdqKCHs1gARWWJeWK5XeNVmt0evBUHYQMMzPb0n0mL25n0ey6%2BnZUBE7cEAL0YO1oEwUOzuze9RfdhogACos5qdiW8I3u2KdgB5ADSEYAYq33fnIzPPfP6XP%2BdrdfqmFQCNFHdmXRa7Rm/QGzMHT6HhyAdnCEWcQISohA5XgFZglSqkzM98uC3SfSWmDLCtq1resQkbZs7gXDse3NPtzgvcMB1HMcXSXBlnXQxkrX1W0SEwLh1RzDCLUFYUwxSMRQ3zCxnRXc1GTTDNUG/EiF27CNowATk1BcyJQBZw05Tk9jMMxUH2ecxIjNwRI44SDlEsxZWIKVGx4gBWNwGHMMxeJ/dD%2BLQf0ZJE3SJKjXTTMU%2BSDjM6S4lUwR1JMLSdLE/SMKwtdrU3bdiEI1iaIFAghRFMVKNoajXTozDPMPCBjwk08QzwHYrxvG47wfNV%2BPtKhMy/ILfx9NLJJ2HiGMLd0jME6y3CUvBJKshSGrKuzFKsxy1Iq1ztN0zyV3pWqTNapSLKk5Sxts2TOocpyCBctyBqqrzV0tdcbUwO1MDMJ0f1I0LhW5IhiElQQZUBGizFIHY1lu6Qdg027A1u51boADlurjbq4eiLSSk1MFGTi9g0qwNMjCBjTwbMQRlfiKIECUDilGVguqnYFBuQhkAQDNYc9FMfVEJQdi4EAF3dUnMDuynMeppgyedem/yLGmKtZn17QIRYGB2JrVrbLAqD9WgCC5j0eb5on/oXIa1tWxkkqDENiFBwFPVkzHriMYHjvxO8CF1hQ7RYCBt1GPdnW1tn3V14B9ZATK7xocXoggVyIahwHVA1V1peIfmIlQTwID93jMOt23ucMR27xd4VjcMU2SHNr3XJ9/1UB2COA8wXmg52TtVEjyNo4XB2ncTkBk4YVPiHThmPQzyHocEAXreJtsPRGoSOrazye6LQP%2BcF7u8yj%2BKSCPbPVfPNKr2ILu5eXAz1pw30t2iPaNQOkKwpO3lUYutZsGva7bvunZHuenZXp2d6di%2BirftXulAct/vo1bqGYbhhGR1wriloOdaUZ9ZaY2xrjfG0MV7Nw5hTKmOwOZrEliTJmtMWbII5lxdBOxR4CyHkWEWYsJaVwLjLYKcUhYtziuvJW9IVYpXPOrcqmt8wxylnHauhsk4mzNhbYGpotaVx4QnPhtcBFpwgDXHKFcdbiINreYU2hdT804WIvWEiVFSJToI5Bs5waZ3bkheBdth5YyAcZfus1B60MsQQyhRdx6GMXMmRkCVmFnkDALdKBCV40ItHODgcxaCcA0rwPwHAtCkFQJwWSlhrBYwWEsWm6weCkAljE0JcwECYCYFgGI6pSAAGsQAaQ0PoTgkgomaF4PEjgvAzhVOyVoOYcBYAwEQAJFgcQ6DRHIJQX4/T6AxGAAoZgpwECoAIHwOg/kzgQAiPU0gERgjVChJwTJvw2CCBHAwWgWycmkCwCiIw4gTn4HtOUYEZwTmYFUGUf0KxYlSmaKs%2BsERrjEChB4LA2zeDGzwCwQFcwqAGAmQANXxCOOIjBAUyEECIMQ7ApBIvkEoNQqzdCNAMEYFA1hrD6BfGcWAzA2AoBYMgUYtAbrAhiMbLwDBSlfjiRFAQ9yQQgkFJJUwSTLBmA0DsEEI41jNOaGUVoLgGDuE8PUfwsqugFCKJkRIyQBBDAaKQLIGqGDKp6DEEYkrygCHaIMeVwwmgtAqGMA1UwjX9A6FqvQowOj2tVVwOYChUnLD0MbTAKweBhIiXUk5jTVAfUDCCQMkgdjAGQMgDMTKWXZggIkqwlhbq4EIDPDJt0PB9IGWw5kXqgX1LZeUyp1SOC1NIKC6t0TYmNOaSAVpFbSCdJ6YJOI/ohkQEwPgU6eo9D8GRaIcQ6Kx2YpUOoE5uLSA3GuHEMFNbImkCbQ0zgI5/S9vDKgdMkbo2xvjYm5NxBmWsozIW0Z0RRJrDLVkiteSClFMoCG2tvAG1VM3XEzgrb205MrRUqp4SODio3asltT6gMfrMGG5t/6YPtLmAypIzhJBAA%3D)


# Wording

## Synopsis

Modify 26.2 Header <ranges> synopsis

::: add
```cpp
  // @[range.maybe]{.sref}@, maybe view
  template<copy_constructible T>
    requires @*see below*@
  class maybe_view;

  template <typename T>
  constexpr inline bool enable_borrowed_range<maybe_view<T*>> = true;

  template <typename T>
  constexpr inline bool enable_borrowed_range<maybe_view<reference_wrapper<T>>> = true;

  namespace views { inline constexpr @_unspecified_@ maybe = @_unspecified_@; }
```
:::

## Maybe View [range.maybe]{.sref}

### Overview
[1]{.pnum} `maybe_view` produces a `view` over a _`nullable`_ that is either empty if the _`nullable`_ is empty, or provides access to the contents of the _`nullable`_ object.

[2]{.pnum} The name `views::maybe` denotes a customization point object ([customization.point.object]). For some subexpression `E`, the expression `views::maybe(E)` is expression-equivalent to:

- `maybe_view{E}`, the `view` specified below, if the expression is well formed, where `@decay-copy@(E)` is moved into the `maybe_view`
- otherwise `views::maybe(E)` is ill-formed.

[Note: Whenever `views::maybe(E)` is a valid expression, it is a prvalue whose type models `view`. — end note ]

[3]{.pnum} [ _Example:_
```cpp
  optional o{4};
  maybe_view m{o};
  for (int i : m)
    cout << i;        // prints 4
```
— _end example_ ]

### Concept *`nullable`*

[1]{.pnum} Types that:

- are contextually convertible to `bool`
- are dereferenceable
- have const references which are dereferenceable
- the `iter_reference_t` of the type and the `iter_reference_t` of the const type, will :
    - satisfy `is_lvalue_reference`
    - satisfy `is_object` when the reference is removed
    - for const pointers to the referred to types, satisfy `convertible_to`
- or are a `reference_wrapper` around a type that satifies _`nullable`_

model the exposition only _`nullable`_ concept

[2]{.pnum} Given a value `i` of type `I`, `I` models _`nullable`_ only if the expression `*i` is equality-preserving. [ Note: The expression `*i` is required to be valid via the exposition-only _`nullable`_ concept). — end note ]

[3]{.pnum} For convienence, the exposition-only `@*is-reference-wrapper-v*@` is used below.
```cpp
// For Exposition
    template <typename T>
    struct is_reference_wrapper : false_type {};

    template <typename T>
    struct is_reference_wrapper<reference_wrapper<T>> : true_type {};

    template <typename T>
    inline constexpr bool is_reference_wrapper_v
        = is_reference_wrapper<T>::value;
```

```cpp
// For Exposition
template <class Ref, class ConstRef>
concept readable_references =
    is_lvalue_reference_v<Ref> &&
    is_object_v<remove_reference_t<Ref>> &&
    is_lvalue_reference_v<ConstRef> &&
    is_object_v<remove_reference_t<ConstRef>> &&
    convertible_to<add_pointer_t<ConstRef>,
                   const remove_reference_t<Ref>*>;

template <class T>
concept nullable =
    is_object_v<T> &&
    requires(T& t, const T& ct) {
        bool(ct); // Contextually bool
        *t; // T& is deferenceable
        *ct; // const T& is deferenceable
    }
    && readable_references<iter_reference_t<T>,        // Ref
                           iter_reference_t<const T>>; // ConstRef

template <class T>
concept wrapped_nullable =
    @*is-reference-wrapper-v*@<T>
    && @_nullable_@<typename T::type>;

```


### Class template *maybe\_view*

```cpp
namespace std::ranges {
  template <copy_constructible Maybe>
    requires (@_nullable_@<Maybe> || @_wrapped-nullable_@<Maybe>)
  class maybe_view : public view_interface<maybe_view<Maybe>> {
  private:
    using T = /* @*see below*@ */
    @*copyable-box*@<Maybe> value_; // exposition only (see [range.copy.wrap])

  public:
    constexpr maybe_view() = default;
    constexpr explicit maybe_view(Maybe const& maybe);

    constexpr explicit maybe_view(Maybe&& maybe);

    template<class... Args>
    requires constructible_from<Maybe, Args...>
    constexpr maybe_view(in_place_t, Args&&... args);

    constexpr T*       begin() noexcept;
    constexpr const T* begin() const noexcept;
    constexpr T*       end() noexcept;
    constexpr const T* end() const noexcept;

    constexpr size_t size() const noexcept;

    constexpr T* data() noexcept;
    constexpr const T* data() const noexcept;
  };
}

```

```cpp
// For Exposition
    using T = std::remove_reference_t<
        iter_reference_t<typename unwrap_reference_t<Maybe>>>;
```

```cpp
constexpr explicit maybe_view(Maybe const& maybe);
```

[1]{.pnum} *Effects*: Initializes value\_ with maybe.

```cpp
constexpr explicit maybe_view(Maybe&& maybe);
```

[2]{.pnum} *Effects*: Initializes value\_ with `std::move(maybe)`.

```cpp
template<class... Args>
constexpr maybe_view(in_place_t, Args&&... args);
```

[3]{.pnum} *Effects*: Initializes value\_ as if by `value_{in_place, forward<Args>(args)...}`.

```cpp
constexpr T* begin() noexcept;
constexpr const T* begin() const noexcept;
```

[4]{.pnum} *Effects*: Equivalent to: `return data();`.

```cpp
constexpr T* end() noexcept;
constexpr const T* end() const noexcept;
```

[5]{.pnum} *Effects*: Equivalent to: `return data() + size();`.

```cpp
static constexpr size_t size() noexcept;
```

[6]{.pnum} *Effects*: Equivalent to:

```cpp
        if constexpr (@*is-reference-wrapper-v*@<Maybe>) {
            return bool(value_.get().get());
        } else {
            return bool(value_.get());
        }
```

🔗

```cpp
constexpr T* data() noexcept;
```

[7]{.pnum} *Effects*: Equivalent to:

```cpp
        Maybe& m = *value_;
        if constexpr (@*is-reference-wrapper-v*@<Maybe>) {
            return m.get() ? addressof(*(m.get())) : nullptr;
        } else {
            return m ? addressof(*m) : nullptr;
        }
```

```cpp
constexpr const T* data() const noexcept;
```

[8]{.pnum} *Effects*: Equivalent to:

```cpp
        const Maybe& m = *value_;
        if constexpr (@*is-reference-wrapper-v*@<Maybe>) {
            return m.get() ? addressof(*(m.get())) : nullptr;
        } else {
            return m ? addressof(*m) : nullptr;
        }
```


# Impact on the standard

A pure library extension, affecting no other parts of the library or language.
