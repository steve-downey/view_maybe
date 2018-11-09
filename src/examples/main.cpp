#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/iterator>
#include <experimental/ranges/concepts>
#include <experimental/ranges/ranges>

#include <array>

// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
namespace ranges = std::experimental::ranges;

template <class...>
class show_type;

template <class Maybe>
concept bool MaybeValue = requires(Maybe m) {
    m ? true : false;
    *m;
};

template <MaybeValue Maybe>
void testMaybe(Maybe const& m);

void checks() {
    testMaybe(std::optional{3});
    //    testMaybe(3);
    std::array ar = {1};
    //    testMaybe(ar);
}

template <MaybeValue Maybe, typename T>
class maybe_view;

template <MaybeValue Maybe, typename T>
requires std::is_object_v<T>&&
    std::is_rvalue_reference_v<Maybe> class maybe_view<Maybe, T>
    : public std::experimental::ranges::view_interface<maybe_view<Maybe, T>> {
  private:
    using M = std::remove_cv_t<std::remove_reference_t<Maybe>>;

    std::experimental::ranges::detail::semiregular_box<M> value_;

  public:
    maybe_view() = default;

    constexpr maybe_view(Maybe maybe) : value_(std::move(maybe)) {}

    constexpr T*       begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T*       end() noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr const T* end() const noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr std::ptrdiff_t size() noexcept {
        if (value_.get())
            return 1;
        else
            return 0;
    }

    constexpr T* data() noexcept {
        if (value_.get())
            return std::addressof(*value_.get());
        else
            return 0;
    }

    constexpr const T* data() const noexcept {
        if (value_.get())
            return std::addressof(*value_.get());
        else
            return 0;
    }
};

template <MaybeValue Maybe, typename T>
requires std::is_object_v<T>&&
    std::is_lvalue_reference_v<Maybe> class maybe_view<Maybe, T> {
    Maybe& value_;
    using R = std::remove_reference_t<decltype(*value_)>;

  public:
    maybe_view() = default;

    constexpr maybe_view(Maybe& maybe) : value_(maybe) {}

    constexpr R*       begin() noexcept { return data(); }
    constexpr const R* begin() const noexcept { return data(); }
    constexpr R*       end() noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr const R* end() const noexcept {
        if (data())
            return data() + 1;
        else
            return data();
    }

    constexpr std::ptrdiff_t size() noexcept {
        if (value_.get())
            return 1;
        else
            return 0;
    }

    constexpr R* data() noexcept {
        if (value_)
            return std::addressof(*value_);
        else
            return 0;
    }

    constexpr const R* data() const noexcept {
        if (value_)
            return std::addressof(*value_);
        else
            return 0;
    }
};

template <class T>
struct dereference {
    typedef std::remove_reference_t<decltype(*std::declval<T>())> type;
};

template <class T>
using dereference_t = typename dereference<T>::type;

template <MaybeValue Maybe>
maybe_view(const Maybe&)->maybe_view<const Maybe&, dereference_t<Maybe>>;

template <MaybeValue Maybe>
maybe_view(Maybe &&)->maybe_view<Maybe&&, dereference_t<Maybe>>;

template <MaybeValue Maybe>
maybe_view(Maybe&)->maybe_view<Maybe&, dereference_t<Maybe>>;

namespace view {
struct __maybe_fn {
    template <MaybeValue T>
    constexpr auto operator()(T&& t) const
        STL2_NOEXCEPT_REQUIRES_RETURN(maybe_view{std::forward<T>(t)})
};

inline constexpr __maybe_fn maybe{};
} // namespace view

template <typename CHECK>
void check(CHECK k) {
    show_type<CHECK> _;
}

template <typename Maybe>
struct test;

template <typename Maybe>
requires std::is_rvalue_reference_v<Maybe> struct test<Maybe> {
    test(Maybe) { std::cout << "is_rvalue_reference_v\n"; }
};

template <typename Maybe>
requires std::is_lvalue_reference_v<Maybe> struct test<Maybe> {
    test(Maybe) { std::cout << "is_lvalue_reference_v\n"; }
};

template <class Maybe>
test(const Maybe&)->test<const Maybe&>;

template <class Maybe>
test(Maybe &&)->test<Maybe&&>;

template <class Maybe>
test(Maybe&)->test<Maybe&>;

int  bar() { return 7; }
int& bar2() {
    static int i = 9;
    return i;
}

class Int {
    int i_;

  public:
    Int() : i_(0){};
    explicit Int(int i) : i_(i){};
    explicit operator int() { return i_; }

    Int operator+(int j) const { return Int(i_ + j); }
};

bool operator==(Int lhs, Int rhs) {
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

bool operator>(Int lhs, Int rhs) {
    return static_cast<int>(lhs) > static_cast<int>(rhs);
}

Int operator+(Int lhs, Int rhs) {
    return Int(static_cast<int>(lhs) + static_cast<int>(rhs));
}

class Double {
    double d_;

  public:
    Double() : d_(0){};
    explicit Double(double d) : d_(d){};
    explicit operator double() { return d_; }
};

bool operator==(Double lhs, Double rhs) {
    return static_cast<double>(lhs) == static_cast<double>(rhs);
};

class NoDefault {
  public:
    NoDefault(int){};
    NoDefault() = delete;
};

class NoCopy {
    NoCopy(NoCopy const&) = delete;

  public:
    NoCopy()         = default;
    NoCopy(NoCopy&&) = default;
};

class NoMove {
    NoMove(NoMove&&) = delete;

  public:
    NoMove()              = default;
    NoMove(NoMove const&) = default;
};

NoDefault makeNoDefault() {
    NoDefault noDefault{1};
    return noDefault;
}

NoMove makeNoMove() {
    NoMove noMove;
    return noMove;
}

NoCopy makeNoCopy() {
    NoCopy noCopy;
    return noCopy;
}

int main() {

    std::optional      s{7};
    std::optional<int> e{};

    maybe_view vs2{s};
    std::cout << *begin(vs2) << '\n';

    for (auto i : vs2)
        std::cout << "i=" << i << '\n'; // prints 4

    for (auto i : view::maybe(s))
        std::cout << "i=" << i << '\n'; // prints 4

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" << i << '\n'; // does not print

    auto oe = std::optional<int>{};
    for (int i : view::maybe(oe))
        std::cout << "i=" << i << '\n'; // does not print

    int        j  = 7;
    int*       pj = &j;
    maybe_view vpj{pj};
    for (auto i : vpj)
        std::cout << "i=" << i << '\n'; // prints 7

    for (auto i : view::maybe(pj))
        std::cout << "i=" << i << '\n'; // prints 7

    std::cout << "j=" << j << '\n'; // prints 7

    for (auto&& i : view::maybe(pj)) {
        i = 27;
        std::cout << "i=" << i << '\n'; // prints 7
    }

    std::cout << "j=" << j << '\n'; // prints 7

    for (auto&& i : view::maybe(s)) {
        i = 9;
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "s=" << *s << '\n'; // prints 7

    for (auto&& i : std::experimental::ranges::view::single(j)) {
        i = 19;
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "j=" << *s << '\n'; // prints 7

    {
        auto&& __range = view::maybe(s);
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }
    std::cout << "s=" << *s << '\n'; // prints 7

    std::array<int, 2> a2 = {2, 3};
    for (auto&& i : view::maybe(a2)) {
        i = 9;
        std::cout << "i=" << i << '\n'; // prints 7
    }

    const std::optional      cs{7};
    const std::optional<int> ce{};

    maybe_view vcs2{cs};
    std::cout << *begin(vcs2) << '\n';

    for (auto&& i : view::maybe(cs)) {
        // i = 9;
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "cs=" << *cs << '\n'; // prints 7

    std::optional<volatile int> vs{7};

    if (vs) {
        std::cout << "*vs = " << *vs << '\n';
    }
    maybe_view vvs2{vs};
    std::cout << *begin(vvs2) << '\n';

    for (auto&& i : view::maybe(vs)) {
        // i = 9;
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "vs=" << *s << '\n'; // prints 7

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : view::maybe(pci)) {
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "pci=" << *pci << '\n'; // prints 7

    auto pvi = &vi;
    std::cout << "pvi=" << *pvi << '\n'; // prints 7
    for (auto&& i : view::maybe(pvi)) {
        i++;
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "pvi=" << *pvi << '\n'; // prints 7

    auto pcvi = &cvi;
    for (auto&& i : view::maybe(pcvi)) {
        std::cout << "i=" << i << '\n'; // prints 7
    }

    int ar[] = {111, 112, 113, 114, 115};
    for (auto&& i : view::maybe(ar)) {
        std::cout << "i=" << i << '\n'; // prints 7
    }

    Int       myInt{231};
    Double    myDouble{457.3};
    NoCopy    noCopy;
    NoMove    noMove;
    NoDefault noDefault{678};

    for (auto&& i : view::maybe(std::optional{myInt})) {
        std::cout << "i=" << int(i) << '\n'; // prints 7
    }

    for (auto&& i : view::maybe(std::optional{myDouble})) {
        std::cout << "i=" << double(i) << '\n'; // prints 7
    }

    std::optional<NoCopy> optionalNoCopy;
    optionalNoCopy.emplace();
    for (auto&& i : view::maybe(optionalNoCopy)) {
        std::cout << "No Copy\n";
    }

    for (auto&& i : view::maybe(std::optional{noMove})) {
        std::cout << "No Move\n";
    }

    for (auto&& i : view::maybe(std::optional{noDefault})) {
        std::cout << "No Default\n";
    }

    int        kkk    = 7;
    int&       r_kkk  = kkk;
    int const& cr_kkk = kkk;
    int&&      rv_kkk = std::move(kkk);

    test to1(std::optional<int>{});
    test to2(std::optional<int>{9});
    test t0(9);
    test t1(kkk);
    test t2(r_kkk);
    test t3(cr_kkk);
    test t4(rv_kkk);
    test t5(bar());
    test t6(bar2());
}
