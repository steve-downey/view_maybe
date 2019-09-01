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
#include <unordered_set>

// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
namespace ranges = std::experimental::ranges;

template <class...>
class show_type;


template <nullable Maybe>
void testMaybe(Maybe const&) {}

template <std::experimental::ranges::readable Read>
void testRead(Read) {}

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
    nullable m{std::optional{3}};
    testRead(std::optional{3});
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

// NoMove makeNoMove() {
//     NoMove noMove;
//     return noMove;
// }

NoCopy makeNoCopy() {
    NoCopy noCopy;
    return noCopy;
}

int func1(int i) {return i;}
int func2(int i) {return 2*i;}

typedef int (*fptr)(int i);

int main() {

    std::unordered_set<int> set{1,3,7,9};

    auto flt = [=](int i) -> std::optional<int> {
        if (set.find(i) != set.end())
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

    std::optional      s{7};
    std::optional<int> e{};

    for (auto i : ranges::views::single(s))
        std::cout << "i=" << *i << " prints 7\n"; // prints 7

    // ref_maybe_view vs2{s};
    // std::cout << *begin(vs2) << " prints 7\n";

    // for (auto i : vs2)
    //     std::cout << "i=" << i << " prints 7\n";

    for (auto i : views::maybe(s))
        std::cout << "i=" << i << " prints 7\n"; // prints 7

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" << i << '\n'; // does not print

    auto oe = std::optional<int>{};
    for (int i : views::maybe(oe))
        std::cout << "i=" << i << '\n'; // does not print

    int        j  = 8;
    int*       pj = &j;
    // ref_maybe_view vpj{pj};
    // for (auto i : vpj)
    //     std::cout << "i=" << i << " prints 8\n"; // prints 8

    for (auto i : views::maybe(pj))
        std::cout << "i=" << i << " prints 8\n"; // prints 8

    std::cout << "j=" << j << " prints 8\n"; // prints 8

    for (auto&& i : views::maybe(pj)) {
        i = 27;
        std::cout << "i=" << i << " prints 27\n"; // prints 27
    }

    std::cout << "j=" << j << " prints 27\n"; // prints 27

    //int _ = views::maybe(std::optional{3});
    for (auto&& i : views::maybe(s)) {
        i = 9;
        std::cout << "i=" << i << " prints 9\n"; // prints 9
    }
    std::cout << "s=" << *s << " prints 9\n"; // prints 9

    for (auto&& i : views::maybe(std::optional{9})) {
        std::cout << "i=" << i << " prints 9\n"; // prints 9
    }
    std::cout << "s=" << *s << " prints 9\n"; // prints 9

    for (auto&& i : std::experimental::ranges::views::single(j)) {
        i = 19;
        std::cout << "i=" << i << " prints 19\n"; // prints 19
    }
    std::cout << "j=" << *s << " prints 9\n"; // prints 9

    {
        auto&& __range = views::maybe(s);
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
    // for (auto&& i : views::maybe(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    // ref_maybe_view vcs2{cs};
    // std::cout << "cs=" << *begin(vcs2) << " prints 3\n";

    for (auto&& i : views::maybe(cs)) {
        //i = 9 //does not compile
        std::cout << "i=" << i << " prints 3\n"; // prints 3
    }
    std::cout << "cs=" << *cs << " prints 3\n"; // prints 3

    for (auto&& i : views::maybe(ce)) {
        //i = 9 //does not compile
        std::cout << "does not print i=" << i << '\n'; // does not print
    }

    std::optional<volatile int> vs{42};

    if (vs) {
        std::cout << "*vs = " << *vs << " prints 42\n";
    }
    // ref_maybe_view vvs2{vs};
    // std::cout << "deref begin vvs=" << *begin(vvs2) << " prints 42\n";

    // for (auto&& i : views::maybe(vs)) {
    //     i = 43;
    //     std::cout << "i=" << i << " prints 43\n"; // prints 43
    // }
    // std::cout << "vs=" << *vs << " prints 43\n"; // prints 43

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : views::maybe(pci)) {
        std::cout << "i=" << i << " prints 11\n"; // prints 11
    }
    std::cout << "pci=" << *pci << " prints 11\n"; // prints 11

    auto pvi = &vi;
    std::cout << "pvi=" << *pvi << " prints 12\n"; // prints 12
    for (auto&& i : views::maybe(pvi)) {
        i++;
        std::cout << "i=" << i << " prints 13\n"; // prints 13
    }
    std::cout << "pvi=" << *pvi << " prints 13\n"; // prints 13

    auto pcvi = &cvi;
    for (auto&& i : views::maybe(pcvi)) {
        // ++i; // does not compile
        std::cout << "i=" << i << " prints 13\n"; // prints 13
    }

    // int ar[] = {111, 112, 113, 114, 115};
    // for (auto&& i : views::maybe(ar)) {
    //     std::cout << "i=" << i << " prints 111\n"; prints 111
    // }

    Int       myInt{231};
    Double    myDouble{457.3};
    NoCopy    noCopy;
    NoMove    noMove;
    NoDefault noDefault{678};

    for (auto&& i : views::maybe(std::optional{myInt})) {
        std::cout << "i=" << int(i) << " prints 231\n"; // prints 231
    }

    for (auto&& i : views::maybe(std::optional{myDouble})) {
        std::cout << "i=" << double(i) << " prints 457.3\n"; // prints 457.3
    }


    for (auto&& i : views::maybe(std::optional{noMove})) {
        std::cout << "No Move" << &i << "\n";
    }

    for (auto&& i : views::maybe(std::optional{noDefault})) {
        std::cout << "No Default" << &i << "\n";
    }

    // Does not compile
    // auto vfunc_view = views::maybe(v_func);
    // auto ifunc_view = views::maybe(i_func);


    // ref_maybe_view copy{pj};
    // copy = vpj;

    std::vector<std::optional<int>> v{std::optional<int>{42},
                                      std::optional<int>{},
                                      std::optional<int>{6 * 9}};

    auto&& x = std::experimental::ranges::views::transform(v, views::maybe);
    for (auto i : x) {
        for (auto j : i)
            std::cout << j << '\t'; // prints 42 and 42
    }
    std::cout << '\n';

    auto r = std::experimental::ranges::views::join(x);

    for (auto i : r) {
        std::cout << i << '\t'; // prints 42 and 42
    }
    std::cout << '\n';

    std::cout << "XYZZY";
    std::cout << '\n';
    using namespace std::experimental;
    for (auto i : ranges::views::join(ranges::views::transform(v, views::maybe))) {
        std::cout << i; // prints 42 and 54
    }

    std::cout << '\n';
    for (auto i : v
             | ranges::views::transform(views::maybe)
             | ranges::views::join) {
        std::cout << i; // prints 42 and 54
    }

    std::cout << '\n';
    for (auto i :
         ranges::iota_view{1, 10}
             | ranges::views::transform(flt)
             | ranges::views::transform([](auto&& o) -> std::optional<int> {
                 if (o) {
                     return *o * *o;
                 }
                 return {};
             })
             | ranges::views::transform(views::maybe)
             | ranges::views::join) {
        std::cout << i; // prints 1 9 49 and 81
    }
    std::cout << '\n';

    std::optional<fptr> f0{};
    std::optional<fptr> f1{func1};
    std::optional<fptr> f2{func2};

    for (auto f : views::maybe(f0)) {
        std::cout << f(1) << '\n';
    }

    for (auto f : views::maybe(f1)) {
        std::cout << f(2) << '\n';
    }

    for (auto f : views::maybe(f2)) {
        std::cout << f(3) << '\n';
    }

    // {
    //     fptr f0 = nullptr;
    //     fptr f1 = &func1;
    //     fptr f2 = &func2;

    //     // for (auto f : views::maybe(f0)) {
    //     //     std::cout << f(1) << '\n';
    //     // }

    //     for (auto f : views::maybe(f1)) {
    //         std::cout << f(2) << '\n';
    //     }

    //     for (auto f : views::maybe(f2)) {
    //         std::cout << f(3) << '\n';
    //     }
    // }
    {
        int i1 = 1;
        int i2 = 2;
        int* p0 = nullptr;
        int* p1 = &i1;
        int* p2 = &i2;

        for (auto f : views::maybe(p0)) {
            std::cout << f << '\n';
        }

        for (auto f : views::maybe(p1)) {
            std::cout << f << '\n';
        }

        for (auto f : views::maybe(p2)) {
            std::cout << f << '\n';
        }

    }
    // {
    //     std::function<int(int)> f0 = nullptr;
    //     std::function<int(int)> f1 = &func1;
    //     std::function<int(int)>f2 = &func2;
    //     //        auto fobj = [](auto f){return [f](int i){return f(i);};};
    //     for (auto f : views::maybe(f0)) {
    //         std::cout << f(1) << '\n';
    //     }

    //     auto w1 = fobj(f1);
    //     for (auto f : views::maybe(f1)) {
    //         std::cout << f(2) << '\n';
    //     }

    //     // for (auto f : views::maybe(f2)) {
    //     //     std::cout << f(3) << '\n';
    //     // }
    // }

    {
        fptr* f0 = nullptr;
        fptr f1 = &func1;
        fptr f2 = &func2;

        for (auto f : std::experimental::ranges::views::single(f0)) {
            //            std::cout << (*f)(1) << '\n';
            //segfault!
        }

        for (auto f : std::experimental::ranges::views::single(f1)) {
            std::cout << f(2) << '\n';
        }

        // for (auto f : views::maybe(f2)) {
        //     std::cout << f(3) << '\n';
        // }
    }

    {
        fptr* f0 = nullptr;
        fptr f1 = &func1;
        fptr* pf1 = &f1;
        fptr f2 = &func2;
        fptr* pf2 = &f2;

        for (auto f : views::maybe(f0)) {
            std::cout << f(1) << '\n';
        }

        for (auto f : views::maybe(pf1)) {
            std::cout << f(2) << '\n';
        }

        for (auto f : views::maybe(pf2)) {
            std::cout << f(3) << '\n';
        }
}

    {
        std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};

        auto&& x = ranges::views::transform(v, [](int i) -> std::optional<int> {
            switch (i) {
            case 1:
            case 3:
            case 7:
            case 9:
                return i;
            default:
                return {};
            }
        });

        auto&& r = ranges::views::transform(
            ranges::views::join(ranges::views::transform(x, views::maybe)),
            [](int i) {
                while (i--) {
                    std::cout << 'a';
                }
                std::cout << '\n';
                return 0;
            });

        for (auto&& i : r) {
        };
    }
}
