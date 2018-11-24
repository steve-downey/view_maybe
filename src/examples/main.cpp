#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/iterator>
#include <experimental/ranges/concepts>
#include <experimental/ranges/ranges>

#include <array>

#include <view_maybe/view_maybe.h>

// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
//namespace ranges = std::experimental::ranges;

template <class...>
class show_type;


template <Nullable Maybe>
void testMaybe(Maybe const&) {}

void v_func();
int i_func();

struct deref {
    int i;
    int const&  operator*() const {return i;}
    explicit operator bool() const { return true; };
};

struct no_ex_bool {
    int i;
    int const&  operator*() const {return i;}
        //    explicit operator bool() const { return true; };
};

void checks() {
    testMaybe(std::optional{3});
    //testMaybe(3);
    // std::array ar = {1};
    // testMaybe(ar);
    int *p;
    testMaybe(p);
    // void *v;
    //     testMaybe(v);
    //    testMaybe(v_func);
    // testMaybe(v_func());

    // bool b = true;
    // testMaybe(b);

    // deref d;
    // testMaybe(d);

    // no_ex_bool neb;
    // testMaybe(neb);
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
    explicit operator int() const { return i_; }

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
    explicit operator double() const { return d_; }
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

    ref_maybe_view vs2{s};
    std::cout << *begin(vs2) << " prints 7\n";

    for (auto i : vs2)
        std::cout << "i=" << i << " prints 7\n";

    for (auto i : view::maybe(s))
        std::cout << "i=" << i << " prints 7\n"; // prints 7

    safe_maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" << i << '\n'; // does not print

    auto oe = std::optional<int>{};
    for (int i : view::maybe(oe))
        std::cout << "i=" << i << '\n'; // does not print

    int        j  = 8;
    int*       pj = &j;
    ref_maybe_view vpj{pj};
    for (auto i : vpj)
        std::cout << "i=" << i << " prints 8\n"; // prints 8

    for (auto i : view::maybe(pj))
        std::cout << "i=" << i << " prints 8\n"; // prints 8

    std::cout << "j=" << j << " prints 8\n"; // prints 8

    for (auto&& i : view::maybe(pj)) {
        i = 27;
        std::cout << "i=" << i << " prints 27\n"; // prints 27
    }

    std::cout << "j=" << j << " prints 27\n"; // prints 27

    //    int _ = view::maybe(s);
    for (auto&& i : view::maybe(s)) {
        i = 9;
        std::cout << "i=" << i << " prints 9\n"; // prints 9
    }
    std::cout << "s=" << *s << " prints 9\n"; // prints 9

    for (auto&& i : view::maybe(std::optional{9})) {
        std::cout << "i=" << i << " prints 9\n"; // prints 9
    }
    std::cout << "s=" << *s << " prints 9\n"; // prints 9

    for (auto&& i : std::experimental::ranges::view::single(j)) {
        i = 19;
        std::cout << "i=" << i << " prints 19\n"; // prints 19
    }
    std::cout << "j=" << *s << " prints 9\n"; // prints 9

    {
        auto&& __range = view::maybe(s);
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }
    std::cout << "s=" << *s << " prints 90\n"; // prints 90

    // Does not compile
    // std::array<int, 2> a2 = {2, 3};
    // for (auto&& i : view::maybe(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    ref_maybe_view vcs2{cs};
    std::cout << "cs=" << *begin(vcs2) << " prints 3\n";

    for (auto&& i : view::maybe(cs)) {
        //i = 9 //does not compile
        std::cout << "i=" << i << " prints 3\n"; // prints 3
    }
    std::cout << "cs=" << *cs << " prints 3\n"; // prints 3

    for (auto&& i : view::maybe(ce)) {
        //i = 9 //does not compile
        std::cout << "does not print i=" << i << '\n'; // does not print
    }

    std::optional<volatile int> vs{42};

    if (vs) {
        std::cout << "*vs = " << *vs << " prints 42\n";
    }
    ref_maybe_view vvs2{vs};
    std::cout << "deref begin vvs=" << *begin(vvs2) << " prints 42\n";

    for (auto&& i : view::maybe(vs)) {
        i = 43;
        std::cout << "i=" << i << " prints 43\n"; // prints 43
    }
    std::cout << "vs=" << *vs << " prints 43\n"; // prints 43

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : view::maybe(pci)) {
        std::cout << "i=" << i << " prints 11\n"; // prints 11
    }
    std::cout << "pci=" << *pci << " prints 11\n"; // prints 11

    auto pvi = &vi;
    std::cout << "pvi=" << *pvi << " prints 12\n"; // prints 12
    for (auto&& i : view::maybe(pvi)) {
        i++;
        std::cout << "i=" << i << " prints 13\n"; // prints 13
    }
    std::cout << "pvi=" << *pvi << " prints 13\n"; // prints 13

    auto pcvi = &cvi;
    for (auto&& i : view::maybe(pcvi)) {
        // ++i; // does not compile
        std::cout << "i=" << i << " prints 13\n"; // prints 13
    }

    // int ar[] = {111, 112, 113, 114, 115};
    // for (auto&& i : view::maybe(ar)) {
    //     std::cout << "i=" << i << " prints 111\n"; // prints 111
    // }

    Int       myInt{231};
    Double    myDouble{457.3};
    NoCopy    noCopy;
    NoMove    noMove;
    NoDefault noDefault{678};

    for (auto&& i : view::maybe(std::optional{myInt})) {
        std::cout << "i=" << int(i) << " prints 231\n"; // prints 231
    }

    for (auto&& i : view::maybe(std::optional{myDouble})) {
        std::cout << "i=" << double(i) << " prints 457.3\n"; // prints 457.3
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
    // auto vfunc_view = view::maybe(v_func);
    // auto ifunc_view = view::maybe(i_func);


    ref_maybe_view copy{pj};
    copy = vpj;

    std::vector<std::optional<int>> v{std::optional<int>{42},
                                      std::optional<int>{},
                                      std::optional<int>{6 * 9}};

    auto&& x = std::experimental::ranges::view::transform(v, view::maybe);
    for (auto i : x) {
        for (auto j : i)
            std::cout << j << '\t'; // prints 42 and 42
    }
    std::cout << '\n';

    auto r = std::experimental::ranges::view::join(x);

    for (auto i : r) {
        std::cout << i << '\t'; // prints 42 and 42
    }
    std::cout << '\n';

}
