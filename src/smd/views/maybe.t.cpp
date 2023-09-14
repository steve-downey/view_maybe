#include <smd/views/maybe.h>

#include <functional>
#include <ranges>
#include <tuple>

#include <gtest/gtest.h>

#include <array>

using namespace smd::views;

TEST(ViewMaybeTest, Constructors) {
    std::ranges::single_view<std::optional<int>> s;
    std::ranges::single_view<std::optional<int>> s2{s};
    std::ranges::single_view<std::optional<int>> s3{std::optional<int>{}};

    maybe_view<std::optional<int>> n;
    maybe_view<std::optional<int>> n2{n};
    maybe_view<std::optional<int>> n3{std::optional<int>{}};
}

TEST(ViewMaybeTest, ConceptCheck) {
    static_assert(std::ranges::range<maybe_view<int>>);
    static_assert(std::ranges::view<maybe_view<int>>);
    static_assert(std::ranges::input_range<maybe_view<int>>);
    static_assert(std::ranges::forward_range<maybe_view<int>>);
    static_assert(std::ranges::bidirectional_range<maybe_view<int>>);
    static_assert(std::ranges::contiguous_range<maybe_view<int>>);
    static_assert(std::ranges::common_range<maybe_view<int>>);
    static_assert(std::ranges::viewable_range<maybe_view<int>>);
    static_assert(!std::ranges::borrowed_range<maybe_view<int>>);

    static_assert(std::ranges::range<maybe_view<int*>>);
    static_assert(std::ranges::view<maybe_view<int*>>);
    static_assert(std::ranges::input_range<maybe_view<int*>>);
    static_assert(std::ranges::forward_range<maybe_view<int*>>);
    static_assert(std::ranges::bidirectional_range<maybe_view<int*>>);
    static_assert(std::ranges::contiguous_range<maybe_view<int*>>);
    static_assert(std::ranges::common_range<maybe_view<int*>>);
    static_assert(std::ranges::viewable_range<maybe_view<int*>>);
    static_assert(std::ranges::borrowed_range<maybe_view<int*>>);

    using ref = std::reference_wrapper<int>;
    static_assert(std::ranges::range<maybe_view<ref>>);
    static_assert(std::ranges::view<maybe_view<ref>>);
    static_assert(std::ranges::input_range<maybe_view<ref>>);
    static_assert(std::ranges::forward_range<maybe_view<ref>>);
    static_assert(std::ranges::bidirectional_range<maybe_view<ref>>);
    static_assert(std::ranges::contiguous_range<maybe_view<ref>>);
    static_assert(std::ranges::common_range<maybe_view<ref>>);
    static_assert(std::ranges::viewable_range<maybe_view<ref>>);
    static_assert(std::ranges::borrowed_range<maybe_view<ref>>);
}

TEST(ViewMaybeTest, ConceptCheckRef) {
    static_assert(std::ranges::range<maybe_view<int&>>);
    static_assert(std::ranges::view<maybe_view<int&>>);
    static_assert(std::ranges::input_range<maybe_view<int&>>);
    static_assert(std::ranges::forward_range<maybe_view<int&>>);
    static_assert(std::ranges::bidirectional_range<maybe_view<int&>>);
    static_assert(std::ranges::contiguous_range<maybe_view<int&>>);
    static_assert(std::ranges::common_range<maybe_view<int&>>);
    static_assert(std::ranges::viewable_range<maybe_view<int&>>);
    static_assert(std::ranges::borrowed_range<maybe_view<int&>>);

    static_assert(std::ranges::range<maybe_view<int*&>>);
    static_assert(std::ranges::view<maybe_view<int*&>>);
    static_assert(std::ranges::input_range<maybe_view<int*&>>);
    static_assert(std::ranges::forward_range<maybe_view<int*&>>);
    static_assert(std::ranges::bidirectional_range<maybe_view<int*&>>);
    static_assert(std::ranges::contiguous_range<maybe_view<int*&>>);
    static_assert(std::ranges::common_range<maybe_view<int*&>>);
    static_assert(std::ranges::viewable_range<maybe_view<int*&>>);
    static_assert(std::ranges::borrowed_range<maybe_view<int*&>>);

    using ref = std::reference_wrapper<int>&;
    static_assert(std::ranges::range<maybe_view<ref>>);
    static_assert(std::ranges::view<maybe_view<ref>>);
    static_assert(std::ranges::input_range<maybe_view<ref>>);
    static_assert(std::ranges::forward_range<maybe_view<ref>>);
    static_assert(std::ranges::bidirectional_range<maybe_view<ref>>);
    static_assert(std::ranges::contiguous_range<maybe_view<ref>>);
    static_assert(std::ranges::common_range<maybe_view<ref>>);
    static_assert(std::ranges::viewable_range<maybe_view<ref>>);
    static_assert(std::ranges::borrowed_range<maybe_view<ref>>);
}

TEST(ViewMaybeTest, BreathingTest) {
    maybe_view<int> m;
    ASSERT_TRUE(m.empty());
    ASSERT_TRUE(m.size() == 0);
    ASSERT_TRUE(m.data() == nullptr);

    maybe_view<int> m1{1};
    ASSERT_TRUE(!m1.empty());
    ASSERT_TRUE(m1.size() == 1);
    ASSERT_TRUE(m1.data() != nullptr);
    ASSERT_TRUE(*(m1.data()) == 1);

    m = m1;
    ASSERT_EQ(*std::begin(m), 1);

    m = {};
    ASSERT_TRUE(m.size() == 0);
    ASSERT_TRUE(m1.size() == 1);

    maybe_view<double> d0{0};
    ASSERT_TRUE(!d0.empty());

    maybe_view<double> d1{1};
    ASSERT_TRUE(!d1.empty());

    d0 = d1;
    ASSERT_EQ(*std::begin(d0), 1.0);
}

TEST(ViewMaybeTest, BreathingTestRef) {
    maybe_view<int&> m;
    ASSERT_TRUE(m.empty());
    ASSERT_TRUE(m.size() == 0);
    ASSERT_TRUE(m.data() == nullptr);

    int              one = 1;
    maybe_view<int&> m1{one};
    ASSERT_TRUE(!m1.empty());
    ASSERT_TRUE(m1.size() == 1);
    ASSERT_TRUE(m1.data() != nullptr);
    ASSERT_TRUE(*(m1.data()) == 1);

    m = m1;
    ASSERT_EQ(*std::begin(m), 1);

    m = {};
    ASSERT_TRUE(m.size() == 0);
    ASSERT_TRUE(m1.size() == 1);

    double              zero = 0.0;
    maybe_view<double&> d0{zero};
    ASSERT_TRUE(!d0.empty());

    double              one_d = 1.0;
    maybe_view<double&> d1{one_d};
    ASSERT_TRUE(!d1.empty());

    d0 = d1;
    ASSERT_EQ(*std::begin(d0), 1.0);
}

TEST(ViewMaybe, CompTest) {
    maybe_view<int> m;
    maybe_view<int> m0{0};
    maybe_view<int> m1{1};
    maybe_view<int> m1a{1};

    ASSERT_EQ(m, m);
    ASSERT_EQ(m0, m0);
    ASSERT_EQ(m1, m1);
    ASSERT_EQ(m1a, m1a);
    ASSERT_EQ(m1, m1a);

    ASSERT_NE(m, m0);
    ASSERT_NE(m0, m1);

    ASSERT_TRUE(m < m0);
    ASSERT_TRUE(m0 < m1);
    ASSERT_TRUE(m1 <= m1a);
}

TEST(ViewMaybe, CompTestRef) {
    maybe_view<int&> m;
    int              zero  = 0;
    int              one   = 1;
    int              one_a = 1;
    maybe_view<int&> m0{zero};
    maybe_view<int&> m1{one};
    maybe_view<int&> m1a{one_a};

    ASSERT_EQ(m, m);
    ASSERT_EQ(m0, m0);
    ASSERT_EQ(m1, m1);
    ASSERT_EQ(m1a, m1a);
    ASSERT_EQ(m1, m1a);

    ASSERT_NE(m, m0);
    ASSERT_NE(m0, m1);

    ASSERT_TRUE(m < m0);
    ASSERT_TRUE(m0 > m);
    ASSERT_TRUE(m0 < m1);
    ASSERT_TRUE(m1a <= m1);
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
    return b ? maybe_view{std::move(x)} : maybe_view<decltype(x)>{};
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

using namespace smd;

TEST(ViewMaybeTest, ValueBase) {
    int             i = 7;
    maybe_view<int> v1{};
    ASSERT_TRUE(v1.size() == 0);

    maybe_view<int> v2{i};
    ASSERT_TRUE(v2.size() == 1);
    for (auto i : v1)
        ASSERT_TRUE(i != i); // tautology so i is used and not warned

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

TEST(ViewMaybeTest, RefWrapper) {
    int i = 7;

    maybe_view<int> v2{std::ref(i)};

    for (auto i : v2)
        ASSERT_EQ(i, 7);

    int s = 4;
    for (auto&& i : views::maybe(std::ref(s))) {
        ASSERT_EQ(i, 4);
        i.get() = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(s, 9);
}

namespace {
class NoDefault {
    int v_;

  public:
    NoDefault(int v) : v_(v) {}
};
} // namespace

TEST(ViewMaybeTest, ValueNonDefaultConstruct) {
    NoDefault             i = 7;
    maybe_view<NoDefault> v1{};
    maybe_view<NoDefault> v2{i};
}

TEST(ViewMaybeTest, RefBase) {
    int              i = 7;
    maybe_view<int&> v1{};
    ASSERT_TRUE(v1.size() == 0);

    maybe_view<int&> v2{i};
    ASSERT_TRUE(v2.size() == 1);
    for (auto i : v1)
        ASSERT_TRUE(i != i); // tautology so i is used and not warned

    for (auto i : v2) {
        ASSERT_EQ(i, 7);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(i, 7);

    for (auto&& i : v2) {
        ASSERT_EQ(i, 7);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(i, 9);

    int s = 4;
    for (auto&& i : views::maybe(s)) {
        ASSERT_EQ(i, 4);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(s, 4);

    for (auto&& i : maybe_view<int&>(s)) {
        ASSERT_EQ(i, 4);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(s, 9);
}

TEST(ViewMaybeTest, MonadicAndThen) {
    maybe_view<int> mv{40};
    auto            r = mv.and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r.empty());
    ASSERT_TRUE(r.size() == 1);
    ASSERT_TRUE(r.data() != nullptr);
    ASSERT_TRUE(*(r.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r2 = mv.and_then([](int) { return maybe_view<int>{}; });
    ASSERT_TRUE(r2.empty());
    ASSERT_TRUE(r2.size() == 0);
    ASSERT_TRUE(r2.data() == nullptr);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    maybe_view<int> empty{};

    auto r3 = empty.and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(r3.empty());
    ASSERT_TRUE(r3.size() == 0);
    ASSERT_TRUE(r3.data() == nullptr);
    ASSERT_TRUE(empty.empty());

    auto r4 = mv.and_then([](double d) { return maybe_view{d + 2}; });
    ASSERT_TRUE(!r4.empty());
    ASSERT_TRUE(r4.size() == 1);
    ASSERT_TRUE(*(r4.data()) == 42.0);
    static_assert(std::is_same_v<decltype(r4), maybe_view<double>>);

    auto r5 = std::move(mv).and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r5.empty());
    ASSERT_TRUE(r5.size() == 1);
    ASSERT_TRUE(r5.data() != nullptr);
    ASSERT_TRUE(*(r5.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r6 = std::move(mv).and_then([](int&& i) {
        int k = i;
        i     = 0;
        return maybe_view{k + 2};
    });
    ASSERT_TRUE(!r6.empty());
    ASSERT_TRUE(r6.size() == 1);
    ASSERT_TRUE(r6.data() != nullptr);
    ASSERT_TRUE(*(r6.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 0);

    const maybe_view<int> cmv{40};
    auto r7 = cmv.and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r7.empty());
    ASSERT_TRUE(r7.size() == 1);
    ASSERT_TRUE(r7.data() != nullptr);
    ASSERT_TRUE(*(r7.data()) == 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);

    auto r8 = std::move(cmv).and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r8.empty());
    ASSERT_TRUE(r8.size() == 1);
    ASSERT_TRUE(r8.data() != nullptr);
    ASSERT_TRUE(*(r8.data()) == 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);
}

TEST(MaybeView, MonadicTransform) {
    maybe_view<int> mv{40};
    auto            r = mv.transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r.empty());
    ASSERT_TRUE(r.size() == 1);
    ASSERT_TRUE(r.data() != nullptr);
    ASSERT_TRUE(*(r.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r2 = mv.transform([](int& i) {
        i += 2;
        return i;
    });
    ASSERT_TRUE(!r2.empty());
    ASSERT_TRUE(r2.size() == 1);
    ASSERT_TRUE(r2.data() != nullptr);
    ASSERT_TRUE(*(r2.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 42);

    maybe_view<int> empty{};

    auto r3 = empty.transform([](int i) { return i + 2; });
    ASSERT_TRUE(r3.empty());
    ASSERT_TRUE(r3.size() == 0);
    ASSERT_TRUE(r3.data() == nullptr);
    ASSERT_TRUE(empty.empty());

    auto r4 = mv.transform([](double d) { return d + 2; });
    ASSERT_TRUE(!r4.empty());
    ASSERT_TRUE(r4.size() == 1);
    ASSERT_TRUE(*(r4.data()) == 44.0);
    static_assert(std::is_same_v<decltype(r4), maybe_view<double>>);

    auto r5 = std::move(mv).transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r5.empty());
    ASSERT_TRUE(r5.size() == 1);
    ASSERT_TRUE(r5.data() != nullptr);
    ASSERT_TRUE(*(r5.data()) == 44);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 42);

    auto r6 = std::move(mv).transform([](int&& i) {
        int k = i;
        i     = 0;
        return k + 2;
    });
    ASSERT_TRUE(!r6.empty());
    ASSERT_TRUE(r6.size() == 1);
    ASSERT_TRUE(r6.data() != nullptr);
    ASSERT_TRUE(*(r6.data()) == 44);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 0);

    const maybe_view<int> cmv{40};
    auto                  r7 = cmv.transform([](int i) { return i + 2; });
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

TEST(MaybeView, MonadicOrElse) {
    maybe_view<int> o1(42);
    auto            r = o1.or_else([] { return maybe_view<int>(13); });
    ASSERT_TRUE(*(r.data()) == 42);

    maybe_view<int> o2;
    ASSERT_TRUE(*(o2.or_else([] { return maybe_view<int>(13); })).data() ==
                13);

    auto r2 = std::move(o1).or_else([] { return maybe_view<int>(13); });
    ASSERT_TRUE(*(r2.data()) == 42);

    auto r3 = std::move(o2).or_else([] { return maybe_view<int>(13); });
    ASSERT_TRUE(*(r3.data()) == 13);
}

TEST(ViewMaybeTest, MonadicAndThenRef) {
    int              forty{40};
    maybe_view<int&> mv{forty};
    auto             r = mv.and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r.empty());
    ASSERT_TRUE(r.size() == 1);
    ASSERT_TRUE(r.data() != nullptr);
    ASSERT_TRUE(*(r.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r2 = mv.and_then([](int) { return maybe_view<int&>{}; });
    ASSERT_TRUE(r2.empty());
    ASSERT_TRUE(r2.size() == 0);
    ASSERT_TRUE(r2.data() == nullptr);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    maybe_view<int&> empty{};

    auto r3 = empty.and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(r3.empty());
    ASSERT_TRUE(r3.size() == 0);
    ASSERT_TRUE(r3.data() == nullptr);
    ASSERT_TRUE(empty.empty());

    auto r4 = mv.and_then([](double d) { return maybe_view{d + 2}; });
    ASSERT_TRUE(!r4.empty());
    ASSERT_TRUE(r4.size() == 1);
    ASSERT_TRUE(*(r4.data()) == 42.0);
    static_assert(std::is_same_v<decltype(r4), maybe_view<double>>);

    auto r5 = std::move(mv).and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r5.empty());
    ASSERT_TRUE(r5.size() == 1);
    ASSERT_TRUE(r5.data() != nullptr);
    ASSERT_TRUE(*(r5.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    auto r6 = std::move(mv).and_then([](int&& i) {
        int k = i;
        i     = 0;
        return maybe_view{k + 2};
    });
    ASSERT_TRUE(!r6.empty());
    ASSERT_TRUE(r6.size() == 1);
    ASSERT_TRUE(r6.data() != nullptr);
    ASSERT_TRUE(*(r6.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 0);
    ASSERT_EQ(forty, 0);
    forty = 40;

    const maybe_view<int&> cmv{forty};
    auto r7 = cmv.and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r7.empty());
    ASSERT_TRUE(r7.size() == 1);
    ASSERT_TRUE(r7.data() != nullptr);
    ASSERT_EQ(*(r7.data()), 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);

    auto r8 = std::move(cmv).and_then([](int i) { return maybe_view{i + 2}; });
    ASSERT_TRUE(!r8.empty());
    ASSERT_TRUE(r8.size() == 1);
    ASSERT_TRUE(r8.data() != nullptr);
    ASSERT_EQ(*(r8.data()), 42);
    ASSERT_TRUE(!cmv.empty());
    ASSERT_TRUE(*(cmv.data()) == 40);
}

TEST(MaybeView, MonadicTransformRef) {
    int              forty{40};
    maybe_view<int&> mv{forty};
    auto             r = mv.transform([](int i) { return i + 2; });
    ASSERT_TRUE(!r.empty());
    ASSERT_TRUE(r.size() == 1);
    ASSERT_TRUE(r.data() != nullptr);
    ASSERT_TRUE(*(r.data()) == 42);
    ASSERT_TRUE(!mv.empty());
    ASSERT_TRUE(*(mv.data()) == 40);

    maybe_view<int&> empty{};

    auto r3 = empty.transform([](int i) { return i + 2; });
    ASSERT_TRUE(r3.empty());
    ASSERT_TRUE(r3.size() == 0);
    ASSERT_TRUE(r3.data() == nullptr);
    ASSERT_TRUE(empty.empty());

    auto r4 = mv.transform([](double d) { return d + 2; });
    ASSERT_TRUE(!r4.empty());
    ASSERT_TRUE(r4.size() == 1);
    ASSERT_TRUE(*(r4.data()) == 42.0);
    static_assert(std::is_same_v<decltype(r4), maybe_view<double>>);

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
    ASSERT_EQ(forty, 0);
    forty = 40;

    const maybe_view<int&> cmv{forty};
    ASSERT_EQ(*(cmv.data()), 40);
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

    auto r9 = mv.transform([](int& i) {
        int k = i;
        i     = 56;
        return k * 2;
    });
    ASSERT_TRUE(!r9.empty());
    ASSERT_EQ(r9.size(), 1);
    ASSERT_TRUE(r9.data() != nullptr);
    ASSERT_EQ(*(r9.data()), 80);
    ASSERT_TRUE(!mv.empty());
    ASSERT_EQ(*(mv.data()), 56);
    ASSERT_EQ(forty, 56);
    forty = 40;
}

TEST(MaybeView, MonadicOrElseRef) {
    int              fortytwo{42};
    int              thirteen{13};
    maybe_view<int&> o1(fortytwo);
    auto r = o1.or_else([&thirteen] { return maybe_view<int&>(thirteen); });
    ASSERT_TRUE(*(r.data()) == 42);

    maybe_view<int&> o2;
    ASSERT_TRUE(*(o2.or_else([&thirteen] {
                     return maybe_view<int&>(thirteen);
                 })).data() == 13);

    auto r2 = std::move(o1).or_else(
        [&thirteen] { return maybe_view<int&>(thirteen); });
    ASSERT_TRUE(*(r2.data()) == 42);

    auto r3 = std::move(o2).or_else(
        [&thirteen] { return maybe_view<int&>(thirteen); });
    ASSERT_TRUE(*(r3.data()) == 13);
}
