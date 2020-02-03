#pragma once
#include <chunx/length_policy.h>

#include <memory>
#include <string_view>

namespace chunx {

template <std::size_t N, typename InputIt = std::string::iterator,
          typename Container = std::string>
class fixed_length_policy
    : public length_policy<fixed_length_policy<N, InputIt, Container>, InputIt,
                           Container> {
 public:
  [[nodiscard]] constexpr std::size_t length() const { return N; };
};

}  // namespace chunx
