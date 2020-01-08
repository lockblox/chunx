#pragma once
#include <chunx/detail/join_iterator.h>

namespace chunx {

template <typename InputIt>
class join_range {
 public:
  join_range(InputIt first, InputIt last);
  using iterator = detail::join_iterator<InputIt>;
  iterator begin() const;
  iterator end() const;

 private:
  InputIt first_;
  InputIt last_;
};

template <typename InputIt>
join_range<InputIt>::join_range(InputIt first, InputIt last)
    : first_(std::move(first)), last_(std::move(last)) {}

template <typename InputIt>
typename join_range<InputIt>::iterator join_range<InputIt>::begin() const {
  return join_range::iterator(first_, last_);
}

template <typename InputIt>
typename join_range<InputIt>::iterator join_range<InputIt>::end() const {
  auto current = std::next(last_, -1);
  return join_range::iterator(first_, last_, last_, current->end());
}

}  // namespace chunx
