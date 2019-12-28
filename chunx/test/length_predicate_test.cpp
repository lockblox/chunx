#include <chunx/length_predicate.h>
#include <chunx/partition_copy.h>
#include <gtest/gtest.h>

#include <algorithm>

using namespace std::string_literals;

class length_predicate_test
    : public testing::TestWithParam<
          std::tuple<std::string, std::size_t, std::vector<std::string>>> {};

TEST_P(length_predicate_test, length_predicate) {
  auto [input, length, expected] = GetParam();
  chunx::length_predicate predicate;
  auto results = std::vector<std::string>{};
  chunx::partition_copy(input.begin(), input.end(), std::back_inserter(results),
                        predicate.length(length));
  EXPECT_EQ(expected, results);
}

TEST(length_predicate_test, string_view) {
  using string_view = std::basic_string_view<int>;
  auto input = std::vector<int>{1, 2, 3, 4, 5, 6};
  auto predicate = chunx::length_predicate{4};
  auto results = std::vector<string_view>{};
  chunx::partition_copy(input.begin(), input.end(), std::back_inserter(results),
                        predicate);
  auto expected = std::vector<string_view>{string_view{input.data(), 4},
                                           string_view{input.data() + 4, 2}};
  EXPECT_EQ(expected, results);
}

INSTANTIATE_TEST_SUITE_P(
    chunker, length_predicate_test,
    testing::Values(
        std::tuple{"123456789"s, 2,
                   std::vector{"12"s, "34"s, "56"s, "78"s, "9"s}},
        std::tuple{"123456789"s, 3, std::vector{"123"s, "456"s, "789"s}},
        std::tuple{"123456789"s, 4, std::vector{"1234"s, "5678"s, "9"s}},
        std::tuple{"123456789"s, 200, std::vector{"123456789"s}}));
