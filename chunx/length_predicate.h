#pragma once
#include <memory>
#include <string_view>

namespace chunx {

class length_predicate {
 public:
  explicit length_predicate(std::size_t length = 262144);
  [[nodiscard]] std::size_t length() const;
  length_predicate& length(std::size_t value);

  /** Determines whether the input is a chunk */
  template <typename T>
  bool operator()(const T& input);

 private:
  std::size_t size_;
};

template <typename T>
bool length_predicate::operator()(const T& input) {
  return length() == input.length();
}

}  // namespace chunx
