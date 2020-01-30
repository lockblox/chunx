#pragma once
#include <chunx/policy.h>

#include <memory>
#include <string_view>

namespace chunx {

template <std::size_t N, typename InputIt = std::string::iterator,
          typename Container = std::string>
class fixed_length_policy : public policy<InputIt, Container> {
 public:
  [[nodiscard]] constexpr std::size_t length() const { return N; };

  bool operator()(InputIt& first, InputIt last, Container& token) override {
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
};

}  // namespace chunx
