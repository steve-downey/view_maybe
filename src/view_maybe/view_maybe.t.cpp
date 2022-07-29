#include <view_maybe/view_maybe.h>
#include <view_maybe/view_maybe.h>

#include <ranges>
#include <tuple>

#include <gtest/gtest.h>

#include <array>

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

    int one = 1;
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

    double zero = 0.0;
    maybe_view<double&> d0{zero};
    ASSERT_TRUE(!d0.empty());

    double one_d = 1.0;
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
    int zero = 0;
    int one = 1;
    int one_a = 1;
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

TEST(ViewMaybeTest, ValueBase) {
    int             i = 7;
    maybe_view<int> v1{};
    ASSERT_TRUE(v1.size() == 0);

    maybe_view<int> v2{i};
    ASSERT_TRUE(v2.size() == 1);
    for (auto i : v1)
        ASSERT_TRUE(false);

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
        ASSERT_TRUE(false);

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
