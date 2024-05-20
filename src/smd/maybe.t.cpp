#include <smd/maybe.h>
#include <smd/maybe.h>

#include <gtest/gtest.h>

#include <optional>
#include <memory>

using namespace smd;

void func();

TEST(MaybeConceptsTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(MaybeConceptsTest, Constructors) {}

TEST(MaybeConceptsTest, Concept) {
    static_assert(maybe<std::optional<int>>);
    static_assert(maybe<std::optional<const int>>);
    static_assert(maybe<std::optional<volatile int>>);
    static_assert(maybe<std::optional<const volatile int>>);

    static_assert(maybe<int*>);
    static_assert(maybe<const int*>);
    static_assert(maybe<int const *>);

    static_assert(maybe<decltype(func)>);

    static_assert(maybe<std::shared_ptr<int>>);
    static_assert(maybe<std::unique_ptr<int>>);
}
