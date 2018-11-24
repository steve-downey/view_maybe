#include <view_maybe/view_maybe.h>

#include <view_maybe/view_maybe.h>

#include <gtest/gtest.h>


#include <experimental/ranges/ranges>
#include <array>

TEST(ViewMaybeTest, TestGTest) {
    ASSERT_EQ(1, 1);
}

TEST(ViewMaybeTest, Concept) {
    static_assert(Nullable<std::optional<int>>);
    static_assert(!Nullable<int>);

    static_assert(Nullable<int*>);
    static_assert(!Nullable<std::array<int,1>>);
    static_assert(!Nullable<void*>);
    // testMaybe(std::optional{3});
        // //testMaybe(3);
        // // std::array ar = {1};
        // // testMaybe(ar);
        // int *p;
        // testMaybe(p);
        // // void *v;
        // //     testMaybe(v);
        // testMaybe(v_func);
        // // testMaybe(v_func());

        // // bool b = true;
        // // testMaybe(b);

        // deref d;
        // testMaybe(d);

        // // no_ex_bool neb;
        // // testMaybe(neb);
}

TEST(ViewMaybeTest, Breathing) {

    std::optional      s{7};
    std::optional<int> e{};

    maybe_view vs2{s};
    ASSERT_EQ(*begin(vs2), 7);

    for (auto i : vs2)
        ASSERT_EQ(i, 7);

    for (auto i : view::maybe(s))
        ASSERT_EQ(i, 7);

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        ASSERT_TRUE(i != i);

    auto oe = std::optional<int>{};
    for (int i : view::maybe(oe))
        ASSERT_TRUE(i != i);

    int        j  = 8;
    int*       pj = &j;
    maybe_view vpj{pj};
    for (auto i : vpj)
        ASSERT_EQ(i, 8);

    for (auto i : view::maybe(pj))
        ASSERT_EQ(i, 8);

    ASSERT_EQ(j, 8);

    for (auto&& i : view::maybe(pj)) {
        i = 27;
        ASSERT_EQ(i, 27);
    }

    ASSERT_EQ(j, 27);

    for (auto&& i : view::maybe(s)) {
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(*s, 9);

    for (auto&& i : std::experimental::ranges::view::single(j)) {
        i = 19;
        ASSERT_EQ(i, 19);
    }
    ASSERT_EQ(j, 27);

    {
        auto&& __range = view::maybe(s);
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }
    ASSERT_EQ(*s, 90);

    // Does not compile
    // std::array<int, 2> a2 = {2, 3};
    // for (auto&& i : view::maybe(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    maybe_view vcs2{cs};
    ASSERT_EQ(*begin(vcs2), 3);

    for (auto&& i : view::maybe(cs)) {
        ASSERT_EQ(i, 3);
    }
    ASSERT_EQ(*cs, 3);

    for (auto&& i : view::maybe(ce)) {
        ASSERT_TRUE(i != i);
    }

    std::optional<volatile int> vs{42};

    if (vs) {
        ASSERT_EQ(*vs, 42);
    }
    maybe_view vvs2{vs};
    ASSERT_EQ(*begin(vvs2), 42);

    for (auto&& i : view::maybe(vs)) {
        i = 43;
        ASSERT_EQ(i, 43);
    }
    ASSERT_EQ(*vs, 43);

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : view::maybe(pci)) {
        ASSERT_EQ(i, 11);
    }
    ASSERT_EQ(*pci, 11);

    auto pvi = &vi;
    ASSERT_EQ(*pvi, 12);
    for (auto&& i : view::maybe(pvi)) {
        i++;
        ASSERT_EQ(i, 13);
    }
    ASSERT_EQ(*pvi, 13);

    auto pcvi = &cvi;
    for (auto&& i : view::maybe(pcvi)) {
        ASSERT_EQ(i, 13);
    }

    int ar[] = {111, 112, 113, 114, 115};
    for (auto&& i : view::maybe(ar)) {
        ASSERT_EQ(i, 111);
    }
}
