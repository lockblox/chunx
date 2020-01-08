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
          typename std::iterator_traits<InputIt>::iterator_category,
          typename std::iterator_traits<InputIt>::value_type::const_reference> {
 public:
  using base_type = boost::iterator_facade<
      join_iterator<InputIt>,
      typename std::iterator_traits<InputIt>::value_type::value_type,
      typename std::iterator_traits<InputIt>::iterator_category,
      typename std::iterator_traits<InputIt>::value_type::const_reference>;

  using difference_type = typename base_type::difference_type;
  using reference = typename base_type::reference;
  using iterator = typename std::iterator_traits<InputIt>::value_type::iterator;

  join_iterator() = default;
  join_iterator(InputIt first, InputIt last);
  join_iterator(InputIt first, InputIt last, InputIt current, iterator value);

  void increment();
  void decrement();
  void advance(difference_type offset);
  bool equal(const join_iterator<InputIt>& other) const;
  reference dereference() const;
  difference_type distance_to(const join_iterator& other) const;

 private:
  difference_type distance_from(const join_iterator& other) const;
  InputIt first_;
  InputIt last_;
  InputIt current_;
  iterator it_;
  difference_type offset_;
};

template <typename InputIt>
join_iterator<InputIt>::join_iterator(InputIt first, InputIt last)
    : join_iterator(first, last, first,
                    first == last ? iterator{} : std::begin(*first)) {}

template <typename InputIt>
join_iterator<InputIt>::join_iterator(InputIt first, InputIt last,
                                      InputIt current, iterator value)
    : first_(std::move(first)),
      last_(std::move(last)),
      current_(std::move(current)),
      it_(std::move(value)),
      offset_(first_ == last_ ||
                      (current_ == first_ && it_ == std::begin(*first_))
                  ? 0
                  : distance_from(join_iterator(first, last))) {}

template <typename InputIt>
void join_iterator<InputIt>::increment() {
  ++offset_;
  if (++it_ == std::end(*current_) && ++current_ != last_)
    it_ = std::begin(*current_);
}

template <typename InputIt>
void join_iterator<InputIt>::decrement() {
  --offset_;
  it_ = it_ == std::begin(*current_) ? std::next(std::end(*--current_), -1)
                                     : --it_;
}

template <typename InputIt>
typename join_iterator<InputIt>::reference join_iterator<InputIt>::dereference()
    const {
  return *it_;
}

template <typename InputIt>
void join_iterator<InputIt>::advance(
    typename join_iterator<InputIt>::difference_type offset) {
  for (auto i = offset; i > 0; --i) {
    increment();
  }
  for (auto i = offset; i < 0; ++i) {
    decrement();
  }
}

template <typename InputIt>
typename join_iterator<InputIt>::difference_type
join_iterator<InputIt>::distance_to(const join_iterator<InputIt>& other) const {
  return other.offset_ - offset_;
}

template <typename InputIt>
typename join_iterator<InputIt>::difference_type
join_iterator<InputIt>::distance_from(
    const join_iterator<InputIt>& other) const {
  difference_type result = 0;
  for (auto i = other.first_; i != current_; ++i) {
    for (auto j = std::begin(*i); j != it_ && j != std::end(*i); ++j) {
      ++result;
    }
  }
  return result;
}

template <typename InputIt>
bool join_iterator<InputIt>::equal(const join_iterator<InputIt>& other) const {
  return first_ == other.first_ && last_ == other.last_ &&
         current_ == other.current_ && it_ == other.it_;
}

}  // namespace detail
}  // namespace chunx
