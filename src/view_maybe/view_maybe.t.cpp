#include <view_maybe/view_maybe.h>

#include <bits/ranges_algo.h>
#include <bits/ranges_base.h>
#include <tuple>
#include <view_maybe/view_maybe.h>

#include <gtest/gtest.h>

#include <ranges>
#include <array>

template <nullable_ref Maybe>
void testMaybe(Maybe const&) {}

TEST(ViewMaybeTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(ViewMaybeTest, Concept) {
    static_assert(nullable<std::optional<int>>);
    static_assert(nullable<std::optional<const int>>);
    static_assert(nullable<std::optional<volatile int>>);
    static_assert(nullable<std::optional<const volatile int>>);

    static_assert(!nullable<int>);

    static_assert(nullable<int*>);
    static_assert(!nullable<std::array<int, 1>>);
    static_assert(!nullable<void*>);

    std::optional<int>                         i;
    std::reference_wrapper<std::optional<int>> t = i;
    testMaybe(t);
    // static_assert(nullable<std::reference_wrapper<std::optional<int>>>);
}

TEST(ViewMaybeTest, Breathing) {

    std::optional      s{7};
    std::optional<int> e{};

    for (auto i : views::maybe(s))
        ASSERT_EQ(i, 7);

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        ASSERT_TRUE(i != i);

    auto oe = std::optional<int>{};
    for (int i : views::maybe(oe))
        ASSERT_TRUE(i != i);

    int  j  = 8;
    int* pj = &j;

    for (auto i : views::maybe(pj))
        ASSERT_EQ(i, 8);

    ASSERT_EQ(j, 8);

    for (auto&& i : views::maybe(pj)) {
        i = 27;
        ASSERT_EQ(i, 27);
    }

    ASSERT_EQ(j, 27);

    int  ixx = 0;
    auto vxx = ranges::views::single(std::ref(ixx));
    for (auto&& jxx : vxx) {
        jxx.get() = 3;
    }
    ASSERT_EQ(ixx, 3);

    for (auto&& i : views::maybe(s)) {
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(*s, 7);

    for (auto&& i : views::maybe(std::ref(s))) {
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(*s, 9);

    for (auto&& i : std::ranges::views::single(j)) {
        i = 19;
        ASSERT_EQ(i, 19);
    }
    ASSERT_EQ(j, 27);

    {
        auto&& __range = views::maybe(s);
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }
    //    ASSERT_EQ(*s, 90);

    // Does not compile
    // std::array<int, 2> a2 = {2, 3};
    // for (auto&& i : views::maybe(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    // ref_maybe_view vcs2{cs};
    // ASSERT_EQ(*begin(vcs2), 3);

    for (auto&& i : views::maybe(cs)) {
        ASSERT_EQ(i, 3);
    }
    ASSERT_EQ(*cs, 3);

    for (auto&& i : views::maybe(ce)) {
        ASSERT_TRUE(i != i);
    }

    std::optional<volatile int> vs{42};

    if (vs) {
        ASSERT_EQ(*vs, 42);
    }
    // ref_maybe_view vvs2{vs};
    // ASSERT_EQ(*begin(vvs2), 42);

    // for (auto&& i : views::maybe(vs)) {
    //     i = 43;
    //     ASSERT_EQ(i, 43);
    // }
    // ASSERT_EQ(*vs, 43);

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : views::maybe(pci)) {
        ASSERT_EQ(i, 11);
    }
    ASSERT_EQ(*pci, 11);

    auto pvi = &vi;
    ASSERT_EQ(*pvi, 12);
    for (auto&& i : views::maybe(pvi)) {
        auto temp = i;
        temp++;
        i = temp;
        ASSERT_EQ(i, 13);
    }
    ASSERT_EQ(*pvi, 13);

    auto pcvi = &cvi;
    for (auto&& i : views::maybe(pcvi)) {
        ASSERT_EQ(i, 13);
    }

    // int ar[] = {111, 112, 113, 114, 115};
    // for (auto&& i : views::maybe(ar)) {
    //     ASSERT_EQ(i, 111);
    // }
}

namespace {
std::optional<int> tempOpt() { return {9}; }

const std::optional<int> tempConstOpt() { return {10}; }

} // namespace
TEST(ViewMaybeTest, RValTest) {

    {
        auto&& __range = views::maybe(tempOpt());
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }

    for (auto&& i : views::maybe(tempOpt())) {
        ++i;
        ASSERT_EQ(i, 10);
    }

    for (auto&& i : views::maybe(tempConstOpt())) {
        ++i;
        ASSERT_EQ(i, 11);
    }
}

TEST(ViewMaybeTest, CVTest) {
    std::optional<int>                o{6};
    std::optional<const int>          co{6};
    std::optional<volatile int>       vo{6};
    std::optional<const volatile int> cvo{6};

    for (auto&& i : views::maybe(o)) {
        ++i;
        ASSERT_EQ(i, 7);
    }
    for (auto&& i : views::maybe(co)) {
        //        ++i;
        ASSERT_EQ(i, 6);
    }
    for (auto&& i : views::maybe(vo)) {
        auto temp = i;
        temp++;
        i = temp;
        ASSERT_EQ(i, 7);
    }
    for (auto&& i : views::maybe(cvo)) {
        //        ++i;
        ASSERT_EQ(i, 6);
    }
}

TEST(ViewMaybeTest, Borrowable) {
    const int num = 42;
    auto      ptr = &num;
    auto      opt = std::optional<int>{42};

    auto found1 = std::ranges::find(views::maybe(std::ref(ptr)), num);
    auto found2 = std::ranges::find(views::maybe(&num), num);
    auto found3 = std::ranges::find(views::maybe(std::ref(opt)), num);

    ASSERT_EQ(*found1, 42);
    ASSERT_EQ(*found2, 42);
    ASSERT_EQ(*found3, 42);
}

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

TEST(ViewMaybeTest, PythTripleTest) {
    using std::ranges::views::iota;
    auto triples = and_then(iota(1), [](int z) {
        return and_then(iota(1, z + 1), [=](int x) {
            return and_then(iota(x, z + 1), [=](int y) {
                return yield_if(x * x + y * y == z * z,
                                std::make_tuple(x, y, z));
            });
        });
    });

    ASSERT_EQ(*std::ranges::begin(triples), std::make_tuple(3, 4, 5));
}

TEST(ViewMaybeTest, ValueBase) {
    int i = 7;
    maybe_view<int>      v1{};
    ASSERT_TRUE(v1.size() == 0);

    maybe_view<int>      v2{i};
    for (auto i : v1)
        ASSERT_TRUE(i != i);

    for (auto i : v2)
        ASSERT_EQ(i, 7);

    int s = 4;
    for (auto&& i : views::maybe(s)) {
        ASSERT_EQ(i, 4);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(s, 4);
}

namespace {
class NoDefault {
    int v_;
  public:
    NoDefault(int v) : v_(v) {}
};
}

TEST(ViewMaybeTest, ValueNonDefaultConstruct) {
    NoDefault       i = 7;
    maybe_view<NoDefault> v1{};
}
