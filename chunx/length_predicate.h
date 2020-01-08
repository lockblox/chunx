#pragma once
#include <memory>
#include <string_view>

namespace chunx {

class length_predicate {
 public:
  explicit length_predicate(std::size_t length = 262144);
  [[nodiscard]] std::size_t length() const;
  length_predicate& length(std::size_t value);

  template <typename InputIt, typename Container>
  bool operator()(InputIt& first, InputIt last, Container& token) {
    using input_type = typename std::iterator_traits<InputIt>::value_type;
    using view_type = typename std::basic_string_view<input_type>;
    auto input_size = std::size_t(std::distance(first, last));
    auto chunk_size = std::min(input_size, length());
    if constexpr (std::is_same_v<Container, view_type>) {
      token = view_type{&*first, chunk_size};
    } else {
      token = Container{first, first + chunk_size};
    }
    std::advance(first, chunk_size);
    return chunk_size > 0;
  }

  /** Determines whether the input is a chunk */
  template <typename T>
  bool operator()(const T& input);

  void reset() {}

 private:
  std::size_t size_;
};

template <typename T>
bool length_predicate::operator()(const T& input) {
  return length() == input.length();
}

}  // namespace chunx
