#include <chunx/fixed_length_policy.h>
#include <chunx/join.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <boost/iterator/transform_iterator.hpp>

namespace test {

struct reverse {
  std::string operator()(const std::string& input) const {
    auto output = input;
    std::reverse(output.begin(), output.end());
    return output;
  }
};

TEST(join_test, transform) {
  using namespace std::string_literals;
  auto input = std::vector{"em"s, "esrever"s};
  auto expected = std::string{"reverseme"};
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

}  // namespace test
