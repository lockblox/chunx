#include <chunx/fixed_length_policy.h>
#include <chunx/join.h>
#include <chunx/split.h>
#include <chunx/variable_length_policy.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>

namespace test {
using namespace std::string_literals;

class length_predicate_test
    : public testing::TestWithParam<
          std::tuple<std::string, std::shared_ptr<chunx::policy<>>,
                     std::vector<std::string>>> {};

// NOLINTNEXTLINE
TEST_P(length_predicate_test, length_policy) {
  auto [input, policy, expected] = GetParam();
  auto splitter = chunx::split(input.begin(), input.end(), *policy);
  auto result = std::vector<std::string>{splitter.begin(), splitter.end()};
  EXPECT_THAT(result, ::testing::Eq(expected));
}

// NOLINTNEXTLINE
TEST(fixed_length_policy, string_view) {
  using string_view = std::basic_string_view<int>;
  // NOLINTNEXTLINE (readability-magic-numbers)
  auto input = std::vector<int>{1, 2, 3, 4, 5, 6};
  auto results = std::vector<string_view>{};
  auto expected = std::vector<string_view>{string_view{input.data(), 4},
                                           string_view{&input[4], 2}};
  auto predicate = chunx::fixed_length_policy<4, std::vector<int>::iterator,
                                              std::basic_string_view<int>>{};
  auto splitter = chunx::split(input.begin(), input.end(), predicate);
  std::copy(splitter.begin(), splitter.end(), std::back_inserter(results));
  EXPECT_EQ(expected, results);
  auto join_range = chunx::join(results.begin(), results.end());
  EXPECT_TRUE(std::equal(input.begin(), input.end(), join_range.begin()));
}

// NOLINTNEXTLINE
TEST(length_policy, op) {
  auto input = std::string{};
  input.resize(3);
  auto predicate = chunx::fixed_length_policy<1>{};
  std::string output;
  auto first = input.begin();

  EXPECT_TRUE(predicate(first, input.end(), output));
  EXPECT_EQ(1, output.size());
  output.clear();

  EXPECT_TRUE(predicate(first, input.end(), output));
  EXPECT_EQ(1, output.size());
  output.clear();

  EXPECT_TRUE(predicate(first, input.end(), output));
  EXPECT_EQ(1, output.size());
  output.clear();

  EXPECT_FALSE(predicate(first, input.end(), output));
  EXPECT_EQ(0, output.size());
}

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(
    chunker, length_predicate_test,
    testing::Values(
        std::tuple{"123456789"s,
                   std::make_shared<chunx::fixed_length_policy<2>>(),
                   std::vector{"12"s, "34"s, "56"s, "78"s, "9"s}},
        std::tuple{"123456789"s,
                   std::make_shared<chunx::fixed_length_policy<3>>(),
                   std::vector{"123"s, "456"s, "789"s}},
        std::tuple{"123456789"s,
                   std::make_shared<chunx::variable_length_policy<>>(3),
                   std::vector{"123"s, "456"s, "789"s}},
        std::tuple{"123456789"s,
                   std::make_shared<chunx::fixed_length_policy<4>>(),
                   std::vector{"1234"s, "5678"s, "9"s}},
        std::tuple{"123456789"s,
                   std::make_shared<chunx::fixed_length_policy<200>>(),
                   std::vector{"123456789"s}},
        std::tuple{"123456789"s,
                   std::make_shared<chunx::fixed_length_policy<0>>(),
                   std::vector{"123456789"s}},
        std::tuple{std::string{'\0', '\0', '\0'},
                   std::make_shared<chunx::fixed_length_policy<1>>(),
                   std::vector{"\0"s, "\0"s, "\0"s}},
        std::tuple{std::string{'\0', '\0', '\0', '\0', '\0', '\0'},
                   std::make_shared<chunx::fixed_length_policy<2>>(),
                   std::vector{"\0\0"s, "\0\0"s, "\0\0"s}}));

}  // namespace test
