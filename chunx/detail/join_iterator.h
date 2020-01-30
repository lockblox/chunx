#pragma once
#include <boost/iterator/iterator_facade.hpp>

namespace chunx {
namespace detail {

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

  join_iterator();
  join_iterator(InputIt first, InputIt last);
  join_iterator(InputIt first, InputIt last, InputIt current,
                std::size_t offset);

  void increment();
  void advance(difference_type offset);
  bool equal(const join_iterator<InputIt>& other) const;
  reference dereference() const;

 private:
  using container_type = typename std::iterator_traits<InputIt>::value_type;
  InputIt first_;
  InputIt last_;
  InputIt current_;
  container_type value_;
  std::size_t offset_;
};

template <typename InputIt>
join_iterator<InputIt>::join_iterator() : offset_(0) {}

template <typename InputIt>
join_iterator<InputIt>::join_iterator(InputIt first, InputIt last)
    : first_(std::move(first)),
      last_(std::move(last)),
      current_(first_),
      value_(current_ == last_ ? container_type{} : *current_),
      offset_(0) {}

template <typename InputIt>
join_iterator<InputIt>::join_iterator(InputIt first, InputIt last,
                                      InputIt current, std::size_t offset)
    : first_(std::move(first)),
      last_(std::move(last)),
      current_(std::move(current)),
      value_(current_ == last_ ? container_type{} : *current_),
      offset_(offset) {}

template <typename InputIt>
void join_iterator<InputIt>::increment() {
  if (++offset_ == std::size(value_)) {
    if (++current_ != last_) {
      value_ = *current_;
      offset_ = 0;
    } else {
      *this = join_iterator{};
    }
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
bool join_iterator<InputIt>::equal(const join_iterator<InputIt>& other) const {
  return first_ == other.first_ && last_ == other.last_ &&
         current_ == other.current_ && offset_ == other.offset_;
}

}  // namespace detail
}  // namespace chunx
