#include <view_maybe/view_maybe.h>

#include <view_maybe/view_maybe.h>

#include <gtest/gtest.h>


#include <experimental/ranges/ranges>
#include <array>

template <nullable_ref Maybe>
void testMaybe(Maybe const&) {}

TEST(ViewMaybeTest, TestGTest) {
    ASSERT_EQ(1, 1);
}

TEST(ViewMaybeTest, Concept) {
    static_assert(nullable<std::optional<int>>);
    static_assert(nullable<std::optional<const int>>);
    static_assert(nullable<std::optional<volatile int>>);
    static_assert(nullable<std::optional<const volatile int>>);

    static_assert(!nullable<int>);

    static_assert(nullable<int*>);
    static_assert(!nullable<std::array<int,1>>);
    static_assert(!nullable<void*>);

    std::optional<int> i;
    std::reference_wrapper<std::optional<int>> t = i;
    testMaybe(t);
    //static_assert(nullable<std::reference_wrapper<std::optional<int>>>);

}

TEST(ViewMaybeTest, Breathing) {

    std::optional      s{7};
    std::optional<int> e{};

    // ref_maybe_view vs2{s};
    // ASSERT_EQ(*begin(vs2), 7);

    // for (auto i : vs2)
    //     ASSERT_EQ(i, 7);

    for (auto i : views::maybe(s))
        ASSERT_EQ(i, 7);

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        ASSERT_TRUE(i != i);

    auto oe = std::optional<int>{};
    for (int i : views::maybe(oe))
        ASSERT_TRUE(i != i);

    int        j  = 8;
    int*       pj = &j;
    // ref_maybe_view vpj{pj};
    // for (auto i : vpj)
    //     ASSERT_EQ(i, 8);

    for (auto i : views::maybe(pj))
        ASSERT_EQ(i, 8);

    ASSERT_EQ(j, 8);

    for (auto&& i : views::maybe(pj)) {
        i = 27;
        ASSERT_EQ(i, 27);
    }

    ASSERT_EQ(j, 27);

    int ixx = 0;
    auto vxx = ranges::views::single(std::ref(ixx));
    for (auto&& jxx : vxx) { jxx.get() = 3;}
    ASSERT_EQ(ixx, 3);

    //    int _ = views::maybe(s);
    // int nope = views::maybe(std::ref(s));
    for (auto&& i : views::maybe(std::ref(s))) {
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(*s, 9);

    for (auto&& i : std::experimental::ranges::views::single(j)) {
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
std::optional<int> tempOpt() {
    return {9};
}

const std::optional<int> tempConstOpt() {
    return {10};
}

}
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
    std::optional<int> o{6};
    std::optional<const int> co{6};
    std::optional<volatile int> vo{6};
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
