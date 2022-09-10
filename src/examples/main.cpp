#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

#include <array>

#include <unordered_set>
#include <view_maybe/view_maybe.h>
#include <view_maybe/view_nullable.h>

// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
namespace ranges = std::ranges;

template <class...>
class show_type;

template <nullable_object Nullable>
void testNullable(const Nullable&) {}

template <std::indirectly_readable Read>
void testRead(Read) {}

void v_func();
int  i_func();

struct deref {
    int        i;
    const int& operator*() const { return i; }
    explicit   operator bool() const { return true; };
};

struct no_ex_bool {
    int        i;
    const int& operator*() const { return i; }
    //    explicit operator bool() const { return true; };
};

void checks() {
    nullable_object auto m{std::optional{3}};
    // testRead(&std::optional{3});
    testNullable(std::optional{3});
    // testNullable(3);
    //  std::array ar = {1};
    //  testNullable(ar);
    int* p;
    testNullable(p);
    // void *v;
    //     testNullable(v);
    //    testNullable(v_func);
    // testNullable(v_func());

    // bool b = true;
    // testNullable(b);

    // deref d;
    // testNullable(d);

    // no_ex_bool neb;
    // testNullable(neb);
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
    NoCopy(const NoCopy&) = delete;

  public:
    NoCopy()         = default;
    NoCopy(NoCopy&&) = default;
};

class NoMove {
    NoMove(NoMove&&) = delete;

  public:
    NoMove()              = default;
    NoMove(const NoMove&) = default;
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

int func1(int i) { return i; }
int func2(int i) { return 2 * i; }

typedef int (*fptr)(int i);

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
    return b ? maybe_view{std::move(x)} : maybe_view<decltype(x)>{};
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

int main() {

    std::unordered_set<int> set{1, 3, 7, 9};

    auto flt = [=](int i) -> std::optional<int> {
        if (set.find(i) != set.end())
            return i;
        else
            return {};
    };

    for (auto i : ranges::iota_view{1, 10} | ranges::views::transform(flt)) {
        for (auto j : views::nullable(i)) {
            for (auto k : ranges::iota_view(0, j))
                std::cout << '\a';
            std::cout << '\n';
        }
    }

    std::optional      s{7};
    std::optional<int> e{};

    for (auto i : ranges::views::single(s))
        std::cout << "i=" << *i << " prints 7\n"; // prints 7

    nullable_view vs2{std::ref(s)};
    std::cout << *begin(vs2) << " prints 7\n";

    for (auto i : vs2)
        std::cout << "i=" << i << " prints 7\n";

    for (auto i : views::nullable(s))
        std::cout << "i=" << i << " prints 7\n"; // prints 7

    nullable_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" << i << '\n'; // does not print

    auto oe = std::optional<int>{};
    for (int i : views::nullable(oe))
        std::cout << "i=" << i << '\n'; // does not print

    int  j  = 8;
    int* pj = &j;

    for (auto i : views::nullable(pj))
        std::cout << "i=" << i << " prints 8\n"; // prints 8

    std::cout << "j=" << j << " prints 8\n"; // prints 8

    for (auto&& i : views::nullable(pj)) {
        i = 27;
        std::cout << "i=" << i << " prints 27\n"; // prints 27
    }

    std::cout << "j=" << j << " prints 27\n"; // prints 27

    for (auto&& i : views::nullable(std::ref(s))) {
        i = 9;
        std::cout << "i=" << i << " prints 9\n"; // prints 9
    }
    std::cout << "s=" << *s << " prints 9\n"; // prints 9

    for (auto&& i : views::nullable(std::optional{9})) {
        std::cout << "i=" << i << " prints 9\n"; // prints 9
    }
    std::cout << "s=" << *s << " prints 9\n"; // prints 9

    for (auto&& i : std::ranges::views::single(j)) {
        i = 19;
        std::cout << "i=" << i << " prints 19\n"; // prints 19
    }
    std::cout << "j=" << *s << " prints 9\n"; // prints 9

    {
        auto&& __range = views::nullable(s);
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
    // for (auto&& i : views::nullable(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    for (auto&& i : views::nullable(cs)) {
        i = 9;
        std::cout << "i=" << i << " prints 9\n"; // prints 3
    }
    std::cout << "cs=" << *cs << " prints 3\n"; // prints 3

    for (auto&& i : views::nullable(ce)) {
        i = 9;
        std::cout << "does not print i=" << i << '\n'; // does not print
    }

    std::optional<volatile int> vs{42};

    if (vs) {
        std::cout << "*vs = " << *vs << " prints 42\n";
    }

    nullable_view vvs2{std::ref(vs)};
    std::cout << "deref begin vvs=" << *begin(vvs2) << " prints 42\n";

    for (auto&& i : views::nullable(vs)) {
        i = 43;
        std::cout << "i=" << i << " prints 43\n"; // prints 43
    }
    std::cout << "vs=" << *vs << " prints 42\n"; // prints 43

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : views::nullable(pci)) {
        std::cout << "i=" << i << " prints 11\n"; // prints 11
    }
    std::cout << "pci=" << *pci << " prints 11\n"; // prints 11

    auto pvi = &vi;
    std::cout << "pvi=" << *pvi << " prints 12\n"; // prints 12
    for (auto&& i : views::nullable(pvi)) {
        i++;
        std::cout << "i=" << i << " prints 13\n"; // prints 13
    }
    std::cout << "pvi=" << *pvi << " prints 13\n"; // prints 13

    auto pcvi = &cvi;
    for (auto&& i : views::nullable(pcvi)) {
        // ++i; // does not compile
        std::cout << "i=" << i << " prints 13\n"; // prints 13
    }

    // int ar[] = {111, 112, 113, 114, 115};
    // for (auto&& i : views::nullable(ar)) {
    //     std::cout << "i=" << i << " prints 111\n"; prints 111
    // }

    Int       myInt{231};
    Double    myDouble{457.3};
    NoCopy    noCopy;
    NoMove    noMove;
    NoDefault noDefault{678};

    for (auto&& i : views::nullable(std::optional{myInt})) {
        std::cout << "i=" << int(i) << " prints 231\n"; // prints 231
    }

    for (auto&& i : views::nullable(std::optional{myDouble})) {
        std::cout << "i=" << double(i) << " prints 457.3\n"; // prints 457.3
    }

    for (auto&& i : views::nullable(std::optional{noMove})) {
        std::cout << "No Move" << &i << "\n";
    }

    for (auto&& i : views::nullable(std::optional{noDefault})) {
        std::cout << "No Default" << &i << "\n";
    }

    // Does not compile
    // auto vfunc_view = views::nullable(v_func);
    // auto ifunc_view = views::nullable(i_func);

    std::vector<std::optional<int>> v{std::optional<int>{42},
                                      std::optional<int>{},
                                      std::optional<int>{6 * 9}};

    auto&& x = std::ranges::views::transform(v, views::nullable);
    for (auto i : x) {
        for (auto j : i)
            std::cout << j << '\t'; // prints 42 and 54
    }
    std::cout << '\n';

    auto r = std::ranges::views::join(x);

    for (auto i : r) {
        std::cout << i << '\t'; // prints 42 and 54
    }
    std::cout << '\n';

    for (auto i :
         ranges::views::join(ranges::views::transform(v, views::nullable))) {
        std::cout << i << '\t'; // prints 42 and 54
    }

    std::cout << '\n';
    for (auto i :
         v | ranges::views::transform(views::nullable) | ranges::views::join) {
        std::cout << i << '\t'; // prints 42 and 54
    }

    std::cout << '\n';
    for (auto i :
         ranges::iota_view{1, 10} | ranges::views::transform(flt) |
             ranges::views::transform([](auto&& o) -> std::optional<int> {
                 if (o) {
                     return *o * *o;
                 }
                 return {};
             }) |
             ranges::views::transform(views::nullable) | ranges::views::join) {
        std::cout << i << '\t'; // prints 1 9 49 and 81
    }
    std::cout << '\n';

    std::optional<fptr> f0{};
    std::optional<fptr> f1{func1};
    std::optional<fptr> f2{func2};

    for (auto f : views::nullable(f0)) {
        std::cout << f(1) << '\n';
    }

    for (auto f : views::nullable(f1)) {
        std::cout << f(2) << '\n';
    }

    for (auto f : views::nullable(f2)) {
        std::cout << f(3) << '\n';
    }

    // Function pointers are not objects
    // Does not compile
    // {
    //     fptr f0 = nullptr;
    //     fptr f1 = &func1;
    //     fptr f2 = &func2;

    //     // for (auto f : views::nullable(f0)) {
    //     //     std::cout << f(1) << '\n';
    //     // }

    //     for (auto f : views::nullable(f1)) {
    //         std::cout << f(2) << '\n';
    //     }

    //     for (auto f : views::nullable(f2)) {
    //         std::cout << f(3) << '\n';
    //     }
    // }
    {
        int  i1 = 1;
        int  i2 = 2;
        int* p0 = nullptr;
        int* p1 = &i1;
        int* p2 = &i2;

        for (auto f : views::nullable(p0)) {
            std::cout << f << '\n';
        }

        for (auto f : views::nullable(p1)) {
            std::cout << f << '\n';
        }

        for (auto f : views::nullable(p2)) {
            std::cout << f << '\n';
        }
    }

    std::cout << "pointers to function pointers\n";

    {
        fptr* f0  = nullptr;
        fptr  f1  = &func1;
        fptr* pf1 = &f1;
        fptr  f2  = &func2;
        fptr* pf2 = &f2;

        for (auto f : views::nullable(f0)) {
            std::cout << f(1) << '\n';
        }

        for (auto f : views::nullable(pf1)) {
            std::cout << f(2) << '\n';
        }

        for (auto f : views::nullable(pf2)) {
            std::cout << f(3) << '\n';
        }
    }

    {
        std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
        auto             test = [](int i) -> std::optional<int> {
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

        auto&& x = ranges::views::transform(v, test);

        auto&& r = ranges::views::transform(
            ranges::views::join(ranges::views::transform(x, views::nullable)),
            [](int i) {
                while (i--) {
                    std::cout << 'a';
                }
                std::cout << '\n';
                return 0;
            });

        for (auto&& i : r) {
        };

        std::cout << '\n';

        auto&& r2 = v | ranges::views::transform(test) |
                    ranges::views::transform(views::nullable) |
                    ranges::views::join | ranges::views::transform([](int i) {
                        std::cout << i;
                        return i;
                    });

        for (auto&& i : r2) {
        };

        std::cout << '\n';

        auto&& r3 = v | ranges::views::transform(test) |
                    ranges::views::filter([](auto x) { return bool(x); }) |
                    ranges::views::transform([](auto x) { return *x; }) |
                    ranges::views::transform([](int i) {
                        std::cout << i;
                        return i;
                    });

        for (auto&& i : r3) {
        };

        std::cout << '\n';
    }
    // {
    //     std::cout << '\n';
    //     std::optional<int>   o   = 99;
    //     auto&&               o_r = std::reference_wrapper(o);
    //     std::vector <std::reference_wrapper<std::optional<int>>> v;
    //     v.emplace_back(o);
    //     for (auto&& i : v) {
    //         std::cout << *(i.get()) << ',';
    //     };
    //     std::cout << '\n';
    //     std::cout << *o << '\n';

    //     auto&& r =
    //         v | ranges::views::transform(views::nullable) |
    //         ranges::views::transform([](auto&& i) { i = i + 1; }) |
    //         ranges::views::transform([](int i) {
    //             std::cout << i;
    //             return i;
    //         });

    //     for (auto&& i : r) {
    //     };
    // }
    print_triples();
}
