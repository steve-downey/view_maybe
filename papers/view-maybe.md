---
title: "A view of 0 or 1 elements: `views::maybe`"
document: P1255R7
date: today
audience: LEWG
author:
  - name: Steve Downey
    email: <sdowney2@bloomberg.net>, <sdowney@gmail.com>
toc: false
---

Abstract: This paper proposes `views::maybe` a range adaptor that produces a view with cardinality 0 or 1 which adapts nullable types such as `std::optional` and pointer to object types.



# Changes
## Changes since R6
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

In writing range transformation it is useful to be able to lift a nullable value into a view that is either empty or contains the value held by the nullable. The adapter `views::single` fills a similar purpose for non-nullable values, lifting a single value into a view, and `views::empty` provides a range of no values of a given type. A `views::maybe` adaptor also allows nullable values to be treated as ranges when it is otherwise undesirable to make them containers, for example `std::optional`.

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


# Proposal

Add a range adaptor object `views::maybe`, returning a view over a nullable object, capturing by value temporary nullables. A _`nullable`_ object is one that is both contextually convertible to bool and for which the type produced by dereferencing is an equality preserving object. Non void pointers, `std::optional`, and the proposed  `expected` [@P0323R9] types all model _`nullable`_. Function pointers do not, as functions are not objects. Iterators do not generally model _`nullable`_, as they are not required to be contextually convertible to bool.


# Design

The basis of the design is to hybridize `views::single` and `views::empty`. If the underlying object claims to hold a value, as determined by checking if the object when converted to bool is true, `begin` and `end` of the view are equivalent to the address of the held value within the underlying object and one past the underlying object. If the underlying object does not have a value, `begin` and `end` return `nullptr`.

# Implementation

A publically available implementation at <https://github.com/steve-downey/view_maybe> based on the Ranges implementation in cmcstl2 at <https://github.com/CaseyCarter/cmcstl2> . There are no particular implementation difficulties or tricks. The declarations are essentially what is quoted in the Wording section and the implementations are described as *Effects*.

[Compiler Explorer Link to Before/After Examples](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAM1QDsCBlZAQwBtMQBGAFlICsupVs1qhkAUgBMAISnTSAZ0ztkBPHUqZa6AMKpWAVwC2tLry3oAMnlqYAcsYBGmYiACsb0gAdUCwuto9QxMzb19/OmtbByNnVw9FZUxVAIYCZmICIONTHkSVNTo0jIIo%2BycXd08FdMzskLyakrKYuKqASkVUA2JkDgByKQBmG2RDLABqcSGdBAICLwUQAHpl4mYAdwA6YEIEA0cDJV66Ai0CLbQjZZrMADdMAFp0VA3bAE9lu7xMDYB9IzMd7OZaA27EG69L4/f6A4GYaG/AFA5xbBDTbDiAAMAEFsTiaugQCBUF5CrQ2NMdDYCBiJj4FH5HOw/nc2AZMBB2tNZLi7qg8OgJkdOTSJgRuUNeTj%2BYKJs4aMRMFiufjxAB2aUTbVTTX4nUTZgGIhSABsZompIIUyGABF6b4mSy2YZOZKtTq8FQJhAre0priDRqPQbtatDRMqL8Ji8AF5aCasGyYBQTLbp/Wh7UiiAAKj9PIm4bwqfzZImAHkANI2gBime1GttDd1zdxTbVfIFQuYVDOxBV/rVetxip9RpNknNU8t5ZAE2%2BvyWIDhzggDKdmFZ7Ld7SH7ZHOIN4eYkejcYTSdsqfTWxbOYLUqLy1n1pLlarw7bePV3/xsqFBUSEwTguQDH9pUJYkrQCNhLWDdUOylfEvR9VB9wglt8xte0AE4eRbKCUG6a0qSpKZJEkVBpmbSibR0cjsLImYKMkeliBpVN8LcHRaCkSQCIPb8iLQY16PI/jqLtfjxJYpiZgkuivA4%2BguPEHi%2BMowSfz/Ls5V7ftQIw4N8SImC6Dg6jNUQ39tLHCAJ2oqcLTwCZ50XDZl1XTkiKVKhfT3YzDwNVyaImfDkMDHURJI2SdFYvAaJk5j4tChSWJk5TOPC9TeP47SO1xGKxJS1ipNotjSvkhiMqUlSCDUjT8sinTOxlbt5UwRVMEkVUD0gggiRAB5VBIKkaTpO5g0kUgJiGWbeAmTwJlNWb1VmgAOWbcNmzgkOlRzxRTUi7SmNxZDcW0IDFPB/UeOkzLJWDWHG%2Bg6RMqLtQUDZCGQBAfVu3UQx1FglAmTgQBbbVQcwObIc%2BkHmDB9V4aPUMYfC1GDSVAgeloCZEparMsCoI1WAILGdRxvGgf2ltCtalr8Ucs0LWIHCF11BiEfWUQU2JDzlwIXmFEVIwIDOGoh3Vbm0e1XngH54aYWXKg8HJlwIHUi6rsOgAPblNWp4h8ccVB9AgA2CN/aXZexkRFeXQXiWFkRRZIcXtfU3XjVQCYraNzBcZNiZcz163bVtlsFaV52QFd2h3eIT2EaDc7veu%2BgCel4Gsy%2BwbiVEk6atS7S89DY38cJ3OdSbd18XslnnJnVz52IHO6fbIS2oAw0%2BxcXruX60yC%2BG5IiGIV7aSGbAF2m2b5omRbltWiZ1omLbwt2zucUOyWTvtL3Lszt87oe0fzIpF6ZgmmfaYR77fv%2B66O9TiYMYhqH36R2Ghkpg0MYoy/hjXC/8JiVwJmXUMJMyYU2jkHGmJlbJE1rrZbuTNcRN2nKacBHM7hc2jg7WOKsXYizFhLY6UceZEKdiQ%2BOZCPYQDjt5KhctwE0IFnQvgAp8YajtlTDhyslykLduQr%2BjZ07Hxuq/Nh5dirF3ItXcRFcEEhyUW/RsNs7IkHHL7VmLc3LgI7sg6UTZ%2BidFYCAfobh%2BikFMP0LEtjUBWIYjIOQEwFDdF6LDYYnBbEUwceYzoCBMDMCwK4LkpAADW7gsRCCsdwWx9jHGkGcf0WxSw4kBMcZ0OAsAYCIGIkYLw6sXDkEoFcEp7BXDAAUBSRYCBUAEFIGrDWxAlgQEcFYlJjgbAZHeFYvxpArhGHOBWWgrABmBNIFgQEoh2DdNsfgJUKQHhLGmZgPWyRjQDCGTSZQiyhB4EcOsYg7w9BYEGf4jiRgrmdBoPQJgbAOB5AEJwIQDsUByDkEcxwSxYAUlGSgIwyAaisBmg8VwwsDC0CiXuVJT06DrMeI8QkNEJBuJkJILEExHgViGBkpIKQNAQAsPUXIpALAtAqK4d5DIIiBH0DkQQ9LyTUtiJUd5SgCipFqFkJlDR8jj15c0ZMrROWKD5eSwQTRMjsraJwTonieh9C4BYqxNi7GHLSXrDappHimm4BMYAyBkA%2BmhbC/0EBcCEB0b42aehimlPZr4/0rjZAyH8Ys%2BFMS3BxMsf0RJpBbl%2Bq1dMtJGSQBZO9aQPJhSSJeGNOUiAlTnWCEwPgCeggHmMBYAsvIGx1heDufE6xSTtVWN8RMH6BB/q6v1Ya41przXEBhXCr1gT4UhLCZUSJAag0hrickpxVjI3Rs7Z0X1/qrEErDSkiNpBslBNLZIct4bR2Lu9Z0SFfgNDcCAA%3D%3D)

# LEWG Attention

~~Call LEWG's attention to the use of `ptrdiff_t` as the return type of `size` (which is consistent with `single_view`). The author has a weak preference for a signed type here, but a strong preference for consistency with other Range types.~~
`single_view` now uses size_t.

Call LEWG's attention to removing the conditional noexcept constructors, consistent with `single_view`.

# Wording

## Synopsis

Modify 24.2 Header <ranges> synopsis

::: add
```cpp
  // @[range.maybe]{.sref}@, maybe view
  template<copy_constructible T>
    requires @*see below*@
  class maybe_view;

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
    @*semiregular-box*@<Maybe> value_; // exposition only (see @[range.semi.wrap]{.sref}@)

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
using T = remove_reference_t<iter_reference_t<typename unwrap_reference_t<Maybe>>>;
```

```cpp
constexpr explicit maybe_view(Maybe const& maybe);
```

[1]{.pnum} *Effects*: Initializes value\_ with maybe.

```cpp
constexpr explicit maybe_view(Maybe&& maybe);
```

[2]{.pnum} *Effects*: Initializes value\_ with `move(maybe)`.

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
static constexpr ptrdiff_t size() noexcept;
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
        Maybe& m = value_.get();
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
        const Maybe& m = value_.get();
        if constexpr (@*is-reference-wrapper-v*@<Maybe>) {
            return m.get() ? addressof(*(m.get())) : nullptr;
        } else {
            return m ? addressof(*m) : nullptr;
        }
```


# Impact on the standard

A pure library extension, affecting no other parts of the library or language.
