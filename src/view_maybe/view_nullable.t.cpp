#include <view_maybe/view_nullable.h>

#include <ranges>
#include <tuple>
#include <view_maybe/view_nullable.h>

#include <gtest/gtest.h>

#include <array>

template <nullable_object_ref NullableRef>
void testNullableRef(NullableRef const&) {}

TEST(ViewNullableTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(ViewNullableTest, Concept) {
    static_assert(nullable_object<std::optional<int>>);
    static_assert(nullable_object<std::optional<const int>>);
    static_assert(nullable_object<std::optional<volatile int>>);
    static_assert(nullable_object<std::optional<const volatile int>>);

    static_assert(!nullable_object<int>);

    static_assert(nullable_object<int*>);
    static_assert(!nullable_object<std::array<int, 1>>);
    static_assert(!nullable_object<void*>);

    std::optional<int>                         i;
    std::reference_wrapper<std::optional<int>> t = i;
    testNullableRef(t);
}

TEST(ViewNullableTest, Breathing) {

    std::optional      s{7};
    std::optional<int> e{};

    for (auto i : views::nullable(s))
        ASSERT_EQ(i, 7);

    nullable_view e2{std::optional<int>{}};
    for (int i : e2)
        ASSERT_TRUE(i != i);

    auto oe = std::optional<int>{};
    for (int i : views::nullable(oe))
        ASSERT_TRUE(i != i);

    int  j  = 8;
    int* pj = &j;

    for (auto i : views::nullable(pj))
        ASSERT_EQ(i, 8);

    ASSERT_EQ(j, 8);

    for (auto&& i : views::nullable(pj)) {
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

    for (auto&& i : views::nullable(s)) {
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(*s, 7);

    for (auto&& i : views::nullable(std::ref(s))) {
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
        auto&& __range = views::nullable(s);
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
    // for (auto&& i : views::nullable(a2)) {
    //     i = 9;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }

    const std::optional      cs{3};
    const std::optional<int> ce{};

    // ref_nullable_view vcs2{cs};
    // ASSERT_EQ(*begin(vcs2), 3);

    for (auto&& i : views::nullable(cs)) {
        ASSERT_EQ(i, 3);
    }
    ASSERT_EQ(*cs, 3);

    for (auto&& i : views::nullable(ce)) {
        ASSERT_TRUE(i != i);
    }

    std::optional<volatile int> vs{42};

    if (vs) {
        ASSERT_EQ(*vs, 42);
    }
    // ref_nullable_view vvs2{vs};
    // ASSERT_EQ(*begin(vvs2), 42);

    // for (auto&& i : views::nullable(vs)) {
    //     i = 43;
    //     ASSERT_EQ(i, 43);
    // }
    // ASSERT_EQ(*vs, 43);

    const int          ci  = 11;
    volatile int       vi  = 12;
    const volatile int cvi = 13;

    auto pci = &ci;
    for (auto&& i : views::nullable(pci)) {
        ASSERT_EQ(i, 11);
    }
    ASSERT_EQ(*pci, 11);

    auto pvi = &vi;
    ASSERT_EQ(*pvi, 12);
    for (auto&& i : views::nullable(pvi)) {
        auto temp = i;
        temp++;
        i = temp;
        ASSERT_EQ(i, 13);
    }
    ASSERT_EQ(*pvi, 13);

    auto pcvi = &cvi;
    for (auto&& i : views::nullable(pcvi)) {
        ASSERT_EQ(i, 13);
    }

    // int ar[] = {111, 112, 113, 114, 115};
    // for (auto&& i : views::nullable(ar)) {
    //     ASSERT_EQ(i, 111);
    // }
}

TEST(ViewNullableTest, BreathingRef) {
    nullable_view<int*&> n;
    ASSERT_TRUE(n.size() == 0);
    int k = 7;
    int* p_k = &k;
    nullable_view<int*&> v_p_k{p_k};
    ASSERT_TRUE(v_p_k.size() == 1);

    std::optional      s{7};
    std::optional<int> e{};

    for (auto i : nullable_view<std::optional<int>&> (s))
        ASSERT_EQ(i, 7);

    nullable_view<std::optional<int>&> e2{e};
    for (int i : e2)
        ASSERT_TRUE(false);

    for (auto&& i : nullable_view<std::optional<int>&>(s)) {
        ASSERT_EQ(i, 7);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(*s, 9);

    auto wrapped_s = std::ref(s);
    for (auto&& i : nullable_view<std::reference_wrapper<std::optional<int>>&>(
             wrapped_s)) {
        i = 19;
        ASSERT_EQ(i, 19);
    }
    ASSERT_EQ(*s, 19);
}

namespace {
std::optional<int> tempOpt() { return {9}; }

const std::optional<int> tempConstOpt() { return {10}; }

} // namespace
TEST(ViewNullableTest, RValTest) {

    {
        auto&& __range = views::nullable(tempOpt());
        auto   __begin = std::begin(__range);
        auto   __end   = std::end(__range);
        for (; __begin != __end; ++__begin) {
            auto&& i = *__begin;
            i        = 90;
        }
    }

    for (auto&& i : views::nullable(tempOpt())) {
        ++i;
        ASSERT_EQ(i, 10);
    }

    for (auto&& i : views::nullable(tempConstOpt())) {
        ++i;
        ASSERT_EQ(i, 11);
    }
}

TEST(ViewNullableTest, CVTest) {
    std::optional<int>                o{6};
    std::optional<const int>          co{6};
    std::optional<volatile int>       vo{6};
    std::optional<const volatile int> cvo{6};

    for (auto&& i : views::nullable(o)) {
        ++i;
        ASSERT_EQ(i, 7);
    }
    for (auto&& i : views::nullable(co)) {
        //        ++i;
        ASSERT_EQ(i, 6);
    }
    for (auto&& i : views::nullable(vo)) {
        auto temp = i;
        temp++;
        i = temp;
        ASSERT_EQ(i, 7);
    }
    for (auto&& i : views::nullable(cvo)) {
        //        ++i;
        ASSERT_EQ(i, 6);
    }
}

TEST(ViewNullableTest, Borrowable) {
    const int num = 42;
    auto      ptr = &num;
    auto      opt = std::optional<int>{42};

    auto found1 = std::ranges::find(views::nullable(std::ref(ptr)), num);
    auto found2 = std::ranges::find(views::nullable(&num), num);
    auto found3 = std::ranges::find(views::nullable(std::ref(opt)), num);

    ASSERT_EQ(*found1, 42);
    ASSERT_EQ(*found2, 42);
    ASSERT_EQ(*found3, 42);
}
