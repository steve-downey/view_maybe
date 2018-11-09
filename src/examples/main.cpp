#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/iterator>
#include <experimental/ranges/concepts>
#include <experimental/ranges/ranges>

#include <array>

#include "view_maybe.h"

// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
//namespace ranges = std::experimental::ranges;

template <class...>
class show_type;


template <Readable Maybe>
void testMaybe(Maybe const&) {}

void v_func();
int i_func();

void checks() {
    testMaybe(std::optional{3});
    //    testMaybe(3);
    //std::array ar = {1};
    //    testMaybe(ar);
    int *p;
    testMaybe(p);
    //void *v;
    //    testMaybe(v);
    testMaybe(v_func);
    //    testMaybe(v_func());
}


template <typename CHECK>
void check(CHECK k) {
    show_type<CHECK> _;
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
}

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
        std::cout << "i=" << i << '\n'; // prints 7

    for (auto i : view::maybe(s))
        std::cout << "i=" << i << '\n'; // prints 7

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" << i << '\n'; // does not print

    auto oe = std::optional<int>{};
    for (int i : view::maybe(oe))
        std::cout << "i=" << i << '\n'; // does not print

    int        j  = 8;
    int*       pj = &j;
    maybe_view vpj{pj};
    for (auto i : vpj)
        std::cout << "i=" << i << '\n'; // prints 8

    for (auto i : view::maybe(pj))
        std::cout << "i=" << i << '\n'; // prints 8

    std::cout << "j=" << j << '\n'; // prints 8

    for (auto&& i : view::maybe(pj)) {
        i = 27;
        std::cout << "i=" << i << '\n'; // prints 27
    }

    std::cout << "j=" << j << '\n'; // prints 27

    for (auto&& i : view::maybe(s)) {
        i = 9;
        std::cout << "i=" << i << '\n'; // prints 9
    }
    std::cout << "s=" << *s << '\n'; // prints 9

    for (auto&& i : std::experimental::ranges::view::single(j)) {
        i = 19;
        std::cout << "i=" << i << '\n'; // prints 19
    }
    std::cout << "j=" << *s << '\n'; // prints 9

    {
        auto&& __range = view::maybe(s);
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }
    std::cout << "s=" << *s << '\n'; // prints 90

    // Does not compile
    // std::array<int, 2> a2 = {2, 3};
    // for (auto&& i : view::maybe(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    maybe_view vcs2{cs};
    std::cout << *begin(vcs2) << '\n';

    for (auto&& i : view::maybe(cs)) {
        // i = 9 does not compile
        std::cout << "i=" << i << '\n'; // prints 3
    }
    std::cout << "cs=" << *cs << '\n'; // prints 3

    std::optional<volatile int> vs{42};

    if (vs) {
        std::cout << "*vs = " << *vs << '\n';
    }
    maybe_view vvs2{vs};
    std::cout << "deref begin vvs=" << *begin(vvs2) << '\n';

    for (auto&& i : view::maybe(vs)) {
        i = 43;
        std::cout << "i=" << i << '\n'; // prints 43
    }
    std::cout << "vs=" << *vs << '\n'; // prints 43

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : view::maybe(pci)) {
        std::cout << "i=" << i << '\n'; // prints 11
    }
    std::cout << "pci=" << *pci << '\n'; // prints 11

    auto pvi = &vi;
    std::cout << "pvi=" << *pvi << '\n'; // prints 12
    for (auto&& i : view::maybe(pvi)) {
        i++;
        std::cout << "i=" << i << '\n'; // prints 13
    }
    std::cout << "pvi=" << *pvi << '\n'; // prints 13

    auto pcvi = &cvi;
    for (auto&& i : view::maybe(pcvi)) {
        // ++i; does not compile
        std::cout << "i=" << i << '\n'; // prints 13
    }

    int ar[] = {111, 112, 113, 114, 115};
    for (auto&& i : view::maybe(ar)) {
        std::cout << "i=" << i << '\n'; // prints 111
    }

    Int       myInt{231};
    Double    myDouble{457.3};
    NoCopy    noCopy;
    NoMove    noMove;
    NoDefault noDefault{678};

    for (auto&& i : view::maybe(std::optional{myInt})) {
        std::cout << "i=" << int(i) << '\n'; // prints 231
    }

    for (auto&& i : view::maybe(std::optional{myDouble})) {
        std::cout << "i=" << double(i) << '\n'; // prints 457.3
    }

    std::optional<NoCopy> optionalNoCopy;
    optionalNoCopy.emplace();
    for (auto&& i: view::maybe(optionalNoCopy)) {
        std::cout << "No Copy" << &i << "\n";
    }

    for (auto&& i : view::maybe(std::optional{noMove})) {
        std::cout << "No Move" << &i << "\n";
    }

    for (auto&& i : view::maybe(std::optional{noDefault})) {
        std::cout << "No Default" << &i << "\n";
    }

    // Does not compile
    // maybe_view vfunc_view{v_func};
    // maybe_view ifunc_view{i_func};

}
