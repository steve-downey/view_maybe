#include <view_maybe/view_nullable.h>
#include <view_maybe/view_nullable.h>

#include <ranges>
#include <tuple>

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

TEST(ViewNullableTest, ConceptCheck) {
    static_assert(std::ranges::range<nullable_view<std::optional<int>>>);
    static_assert(std::ranges::view<nullable_view<std::optional<int>>>);
    static_assert(std::ranges::input_range<nullable_view<std::optional<int>>>);
    static_assert(
        std::ranges::forward_range<nullable_view<std::optional<int>>>);
    static_assert(
        std::ranges::bidirectional_range<nullable_view<std::optional<int>>>);
    static_assert(
        std::ranges::contiguous_range<nullable_view<std::optional<int>>>);
    static_assert(
        std::ranges::common_range<nullable_view<std::optional<int>>>);
    static_assert(
        std::ranges::viewable_range<nullable_view<std::optional<int>>>);
    static_assert(
        !std::ranges::borrowed_range<nullable_view<std::optional<int>>>);

    static_assert(std::ranges::range<nullable_view<int*>>);
    static_assert(std::ranges::view<nullable_view<int*>>);
    static_assert(std::ranges::input_range<nullable_view<int*>>);
    static_assert(std::ranges::forward_range<nullable_view<int*>>);
    static_assert(std::ranges::bidirectional_range<nullable_view<int*>>);
    static_assert(std::ranges::contiguous_range<nullable_view<int*>>);
    static_assert(std::ranges::common_range<nullable_view<int*>>);
    static_assert(std::ranges::viewable_range<nullable_view<int*>>);
    static_assert(std::ranges::borrowed_range<nullable_view<int*>>);

    using ref = std::reference_wrapper<std::optional<int>>;
    static_assert(std::ranges::range<nullable_view<ref>>);
    static_assert(std::ranges::view<nullable_view<ref>>);
    static_assert(std::ranges::input_range<nullable_view<ref>>);
    static_assert(std::ranges::forward_range<nullable_view<ref>>);
    static_assert(std::ranges::bidirectional_range<nullable_view<ref>>);
    static_assert(std::ranges::contiguous_range<nullable_view<ref>>);
    static_assert(std::ranges::common_range<nullable_view<ref>>);
    static_assert(std::ranges::viewable_range<nullable_view<ref>>);
    static_assert(std::ranges::borrowed_range<nullable_view<ref>>);
}

TEST(ViewNullableTest, ConceptCheckRef) {
    static_assert(std::ranges::range<nullable_view<std::optional<int>&>>);
    static_assert(std::ranges::view<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::input_range<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::forward_range<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::bidirectional_range<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::contiguous_range<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::common_range<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::viewable_range<nullable_view<std::optional<int>&>>);
    static_assert(
        std::ranges::borrowed_range<nullable_view<std::optional<int>&>>);

    static_assert(std::ranges::range<nullable_view<int*&>>);
    static_assert(std::ranges::view<nullable_view<int*&>>);
    static_assert(std::ranges::input_range<nullable_view<int*&>>);
    static_assert(std::ranges::forward_range<nullable_view<int*&>>);
    static_assert(std::ranges::bidirectional_range<nullable_view<int*&>>);
    static_assert(std::ranges::contiguous_range<nullable_view<int*&>>);
    static_assert(std::ranges::common_range<nullable_view<int*&>>);
    static_assert(std::ranges::viewable_range<nullable_view<int*&>>);
    static_assert(std::ranges::borrowed_range<nullable_view<int*&>>);

    using ref = std::reference_wrapper<std::optional<int>>&;
    static_assert(std::ranges::range<nullable_view<ref>>);
    static_assert(std::ranges::view<nullable_view<ref>>);
    static_assert(std::ranges::input_range<nullable_view<ref>>);
    static_assert(std::ranges::forward_range<nullable_view<ref>>);
    static_assert(std::ranges::bidirectional_range<nullable_view<ref>>);
    static_assert(std::ranges::contiguous_range<nullable_view<ref>>);
    static_assert(std::ranges::common_range<nullable_view<ref>>);
    static_assert(std::ranges::viewable_range<nullable_view<ref>>);
    static_assert(std::ranges::borrowed_range<nullable_view<ref>>);
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

TEST(ViewNullableTest, Transform) {
    nullable_view<std::optional<int>> v{50};
    auto r = std::ranges::views::transform(v, [](int i){return i * 2.0;});
    for(auto && d : r) {
        ASSERT_EQ(d, 100.0);
    }
}

TEST(ViewNullableTest, BreathingRef) {
    nullable_view<int*&> n;
    ASSERT_TRUE(n.size() == 0);
    int                  k   = 7;
    int*                 p_k = &k;
    nullable_view<int*&> v_p_k{p_k};
    ASSERT_TRUE(v_p_k.size() == 1);

    std::optional      s{7};
    std::optional<int> e{};

    for (auto i : nullable_view<std::optional<int>&>(s))
        ASSERT_EQ(i, 7);

    nullable_view<std::optional<int>&> e2{e};
    for (int i : e2)
        ASSERT_TRUE(i != i); // tautology to avoid unused variable warning

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

TEST(ViewNullable, CompTest) {
    int zero  = 0;
    int one   = 1;
    int one_a = 1;

    nullable_view<int*> m;
    nullable_view<int*> mz{nullptr};
    nullable_view<int*> m0{&zero};
    nullable_view<int*> m1{&one};
    nullable_view<int*> m1a{&one_a};

    ASSERT_EQ(m, m);
    ASSERT_EQ(m, mz);
    ASSERT_EQ(m0, m0);
    ASSERT_EQ(m1, m1);
    ASSERT_EQ(m1a, m1a);
    ASSERT_EQ(m1, m1a);

    ASSERT_NE(m, m0);
    ASSERT_NE(m0, m1);
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

TEST(ViewNullableTest, MonadicAndThen) {
    std::optional<int>                forty{40};
    nullable_view<std::optional<int>> mv{forty};

    auto r = mv.and_then(
        [](int i) { return nullable_view<std::optional<int>>{i + 2}; });
    ASSERT_TRUE(!r.empty());
    ASSERT_TRUE(r.size() == 1);
    ASSERT_TRUE(r.data() != nullptr);
    ASSERT_TRUE(*(r.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r2 =
        mv.and_then([](int) { return nullable_view<std::optional<int>>{}; });
    ASSERT_TRUE(r2.empty());
    ASSERT_TRUE(r2.size() == 0);
    ASSERT_TRUE(r2.data() == nullptr);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    std::optional<int> e;
    nullable_view      empty{e};

    auto r3 = empty.and_then(
        [](int i) { return nullable_view<std::optional<int>>{i + 2}; });
    ASSERT_TRUE(r3.empty());
    ASSERT_TRUE(r3.size() == 0);
    ASSERT_TRUE(r3.data() == nullptr);
    ASSERT_TRUE(empty.empty());

    auto r4 = mv.and_then(
        [](double d) { return nullable_view<std::optional<double>>{d + 2}; });
    ASSERT_TRUE(!r4.empty());
    ASSERT_TRUE(r4.size() == 1);
    ASSERT_TRUE(*(r4.data()) == 42.0);
    static_assert(
        std::is_same_v<decltype(r4), nullable_view<std::optional<double>>>);

    auto r5 = std::move(mv).and_then(
        [](int i) { return nullable_view<std::optional<int>>{i + 2}; });
    ASSERT_TRUE(!r5.empty());
    ASSERT_TRUE(r5.size() == 1);
    ASSERT_TRUE(r5.data() != nullptr);
    ASSERT_TRUE(*(r5.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r6 = std::move(mv).and_then([](int&& i) {
        int k = i;
        i     = 0;
        return nullable_view<std::optional<int>>{k + 2};
    });
    ASSERT_TRUE(!r6.empty());
    ASSERT_TRUE(r6.size() == 1);
    ASSERT_TRUE(r6.data() != nullptr);
    ASSERT_TRUE(*(r6.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 0);

    const nullable_view<std::optional<int>> cmv{40};
    auto                                    r7 = cmv.and_then(
        [](int i) { return nullable_view<std::optional<int>>{i + 2}; });
    ASSERT_TRUE(!r7.empty());
    ASSERT_TRUE(r7.size() == 1);
    ASSERT_TRUE(r7.data() != nullptr);
    ASSERT_TRUE(*(r7.data()) == 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);

    auto r8 = std::move(cmv).and_then(
        [](int i) { return nullable_view<std::optional<int>>{i + 2}; });
    ASSERT_TRUE(!r8.empty());
    ASSERT_TRUE(r8.size() == 1);
    ASSERT_TRUE(r8.data() != nullptr);
    ASSERT_TRUE(*(r8.data()) == 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);
}

TEST(NullableView, MonadicTransform) {
    nullable_view<std::optional<int>> mv{40};
    auto r = mv.transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r.empty());
    ASSERT_TRUE(r.size() == 1);
    ASSERT_TRUE(r.data() != nullptr);
    ASSERT_TRUE(*(r.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    nullable_view<std::optional<int>> empty{};

    auto r3 = empty.transform([](int i) { return i + 2; });
    ASSERT_TRUE(r3.empty());
    ASSERT_TRUE(r3.size() == 0);
    ASSERT_TRUE(r3.data() == nullptr);
    ASSERT_TRUE(empty.empty());

    auto r4 = mv.transform([](double d) { return d + 2; });
    ASSERT_TRUE(!r4.empty());
    ASSERT_TRUE(r4.size() == 1);
    ASSERT_TRUE(*(r4.data()) == 42.0);
    int _ = r4;
    static_assert(
        std::is_same_v<decltype(r4), nullable_view<std::optional<double>>>);

    auto r5 = std::move(mv).transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r5.empty());
    ASSERT_TRUE(r5.size() == 1);
    ASSERT_TRUE(r5.data() != nullptr);
    ASSERT_TRUE(*(r5.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r6 = std::move(mv).transform([](int&& i) {
        int k = i;
        i     = 0;
        return k + 2;
    });
    ASSERT_TRUE(!r6.empty());
    ASSERT_TRUE(r6.size() == 1);
    ASSERT_TRUE(r6.data() != nullptr);
    ASSERT_TRUE(*(r6.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 0);

    const nullable_view<std::optional<int>> cmv{40};
    auto r7 = cmv.transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r7.empty());
    ASSERT_TRUE(r7.size() == 1);
    ASSERT_TRUE(r7.data() != nullptr);
    ASSERT_TRUE(*(r7.data()) == 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);

    auto r8 = std::move(cmv).transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r8.empty());
    ASSERT_TRUE(r8.size() == 1);
    ASSERT_TRUE(r8.data() != nullptr);
    ASSERT_TRUE(*(r8.data()) == 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);
}

TEST(NullableView, MonadicOrElse) {
    nullable_view<std::optional<int>> o1{42};
    auto r = o1.or_else([] { return nullable_view<std::optional<int>>(13); });
    ASSERT_TRUE(*(r.data()) == 42);

    nullable_view<std::optional<int>> o2{};
    ASSERT_TRUE(*(o2.or_else([] {
                     return nullable_view<std::optional<int>>(13);
                 })).data() == 13);

    auto r2 = std::move(o1).or_else(
        [] { return nullable_view<std::optional<int>>(13); });
    ASSERT_TRUE(*(r2.data()) == 42);

    auto r3 = std::move(o2).or_else(
        [] { return nullable_view<std::optional<int>>(13); });
    ASSERT_TRUE(*(r3.data()) == 13);
}
