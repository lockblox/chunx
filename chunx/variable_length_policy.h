#pragma once
#include <chunx/length_policy.h>

namespace chunx {

template <typename InputIt = std::string::iterator,
          typename Container = std::string>
class variable_length_policy
    : public length_policy<variable_length_policy<InputIt, Container>, InputIt,
                           Container> {
 public:
  explicit variable_length_policy(std::size_t length) : length_(length) {}

  void length(std::size_t length) { length_ = length; }

  [[nodiscard]] constexpr std::size_t length() const { return length_; };

 private:
  std::size_t length_;
};

}  // namespace chunx
