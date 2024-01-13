#include <smd/views/movable_box.h>

#include <smd/views/movable_box.h>

#include <gtest/gtest.h>

#include <ranges>
#include <optional>

TEST(Movable_BoxTest, TestGTest) { ASSERT_EQ(1, 1); }

namespace {
class NoDefault {
    int v_;

  public:
    NoDefault(int v) : v_(v) {}
};
} // namespace

TEST(Movable_BoxTest, Constructors) {
  smd::views::detail::movable_box<int> mb1{};
  smd::views::detail::movable_box<int> mb2{1};
  smd::views::detail::movable_box<int> mb3{mb1};

}
