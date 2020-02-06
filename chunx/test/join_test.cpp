#include <chunx/fixed_length_policy.h>
#include <chunx/join.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <boost/iterator/transform_iterator.hpp>

namespace test {

using namespace std::string_literals;

struct reverse {
  std::string operator()(const std::string& input) const {
    std::string output;
    if (!input.empty()) {
      output = input;
      std::reverse(output.begin(), output.end());
    }
    return output;
  }
};

class join_test : public testing::TestWithParam<
                      std::pair<std::vector<std::string>, std::string>> {};

TEST_P(join_test, transform) {
  using namespace std::string_literals;
  auto [input, expected] = GetParam();
  using transform_iterator =
      boost::transform_iterator<reverse,
                                std::vector<std::string>::reverse_iterator>;
  auto f = reverse{};
  auto begin = transform_iterator{input.rbegin(), f};
  auto end = transform_iterator{input.rend(), f};
  auto joined = chunx::join(begin, end);
  auto result = std::string{joined.begin(), joined.end()};
  EXPECT_THAT(result, ::testing::Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(
    chunker, join_test,
    testing::Values(std::pair{std::vector{""s, "em"s, ""s, "esrever"s, ""s},
                              "reverseme"s},
                    std::pair{std::vector{"em"s, "esrever"s}, "reverseme"s},
                    std::pair{std::vector<std::string>{}, std::string{}}));
}  // namespace test
