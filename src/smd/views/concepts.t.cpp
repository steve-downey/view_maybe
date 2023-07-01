#include <smd/views/concepts.h>

#include <smd/views/concepts.h>

#include <gtest/gtest.h>

#include <ranges>
#include <optional>

using namespace smd::views;

void func();

TEST(ConceptsTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(ConceptsTest, Constructors) {}

TEST(ConceptsTest, Concept) {
    static_assert(nullable_object<std::optional<int>>);
    static_assert(nullable_object<std::optional<const int>>);
    static_assert(nullable_object<std::optional<volatile int>>);
    static_assert(nullable_object<std::optional<const volatile int>>);

    static_assert(!nullable_object<int>);

    static_assert(nullable_object<int*>);
    static_assert(!nullable_object<std::array<int, 1>>);
    static_assert(!nullable_object<void*>);
    static_assert(!nullable_object<decltype(func)>);

    static_assert(
        nullable_object_ref<std::reference_wrapper<std::optional<int>>>);
}
