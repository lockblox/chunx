#pragma once
#include <chunx/detail/join_iterator.h>

#include <boost/range/iterator_range.hpp>

namespace chunx {

template <typename InputIt>
boost::iterator_range<detail::join_iterator<InputIt>> join(InputIt first,
                                                           InputIt last);

template <typename InputIt>
boost::iterator_range<detail::join_iterator<InputIt>> join(InputIt first,
                                                           InputIt last) {
  using iterator = detail::join_iterator<InputIt>;
  using result_type = boost::iterator_range<iterator>;
  return result_type{iterator{first, last}, iterator{last, last}};
}

}  // namespace chunx
