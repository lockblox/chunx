#pragma once
#include <chunx/policy.h>

namespace chunx {

template <typename Derived, typename InputIt = std::string::iterator,
          typename Container = std::string>
class length_policy : public policy<InputIt, Container> {
 public:
  [[nodiscard]] std::size_t get_length() const {
    return static_cast<const Derived*>(this)->length();
  };

  bool operator()(InputIt& first, InputIt last, Container& token) override {
    using input_type = typename std::iterator_traits<InputIt>::value_type;
    using view_type = typename std::basic_string_view<input_type>;
    auto input_size = std::size_t(std::distance(first, last));
    auto size = get_length();
    size = size == 0 ? input_size : std::min(input_size, size);
    if constexpr (std::is_same_v<Container, view_type>) {
      token = view_type{&*first, size};
    } else {
      token = Container{first, std::next(first, size)};
    }
    std::advance(first, size);
    return size > 0;
  }
};  // namespace chunx

}  // namespace chunx
