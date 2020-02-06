#pragma once
#include <boost/iterator/iterator_facade.hpp>

namespace chunx::detail {

/** Converts a range of iterators-to-containers, to an iterator to values */
template <typename InputIt>
class join_iterator
    : public boost::iterator_facade<
          join_iterator<InputIt>,
          typename std::iterator_traits<InputIt>::value_type::value_type,
          std::forward_iterator_tag,
          typename std::iterator_traits<InputIt>::value_type::const_reference> {
 public:
  using base_type = boost::iterator_facade<
      join_iterator<InputIt>,
      typename std::iterator_traits<InputIt>::value_type::value_type,
      std::forward_iterator_tag,
      typename std::iterator_traits<InputIt>::value_type::const_reference>;

  using difference_type = typename base_type::difference_type;
  using reference = typename base_type::reference;
  using iterator =
      typename std::iterator_traits<InputIt>::value_type::const_iterator;

  join_iterator(InputIt first, InputIt last);

  void increment();
  void advance(difference_type offset);
  bool equal(const join_iterator<InputIt> &other) const;
  reference dereference() const;

 private:
  using container_type = typename std::iterator_traits<InputIt>::value_type;
  InputIt current_;
  InputIt last_;
  container_type value_;
  std::size_t offset_;
};

template <typename InputIt>
join_iterator<InputIt>::join_iterator(InputIt first, InputIt last)
    : current_(std::move(first)),
      last_(std::move(last)),
      value_(current_ == last_ ? container_type{} : *current_),
      offset_(0) {
  while (std::size(value_) == 0 && current_ != last_) {
    increment();
  }
}

template <typename InputIt>
void join_iterator<InputIt>::increment() {
  auto size = std::size(value_);
  if (offset_ < size) {
    ++offset_;
  }
  while (offset_ == std::size(value_) && current_ != last_) {
    ++current_;
    value_ = current_ == last_ ? container_type{} : *current_;
    offset_ = 0;
  }
}

template <typename InputIt>
typename join_iterator<InputIt>::reference join_iterator<InputIt>::dereference()
    const {
  return value_[offset_];
}

template <typename InputIt>
void join_iterator<InputIt>::advance(
    typename join_iterator<InputIt>::difference_type offset) {
  for (auto i = offset; i > 0; --i) {
    increment();
  }
}

template <typename InputIt>
bool join_iterator<InputIt>::equal(const join_iterator<InputIt> &other) const {
  return current_ == other.current_ && last_ == other.last_ &&
         offset_ == other.offset_;
}

}  // namespace chunx
