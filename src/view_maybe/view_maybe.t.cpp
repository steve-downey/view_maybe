#include <view_maybe/view_maybe.h>

#include <ranges>
#include <tuple>
#include <view_maybe/view_maybe.h>

#include <gtest/gtest.h>

#include <array>

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
}

TEST(ViewMaybeTest, RefBase) {
    int              i = 7;
    maybe_view<int&> v1{};
    ASSERT_TRUE(v1.size() == 0);

    maybe_view<int&> v2{i};
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
}
