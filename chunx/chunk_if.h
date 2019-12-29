#pragma once

#include <chunx/length_predicate.h>

#include <boost/iterator/iterator_facade.hpp>
#include <string_view>
#include <type_traits>

namespace chunx {

/** Copies an input range into a sequence of chunks for which a predicate is
 * true */
template <typename InputIt, typename OutputIt, typename UnaryFunction>
OutputIt chunk_if(InputIt first, InputIt last, OutputIt d_first,
                  UnaryFunction p) {
  using input_type = typename std::iterator_traits<InputIt>::value_type;
  using input_tag = typename std::iterator_traits<InputIt>::iterator_category;
  using view_type = typename std::basic_string_view<input_type>;
  // TODO use trait class to determine output type
  using output_type = typename OutputIt::container_type::value_type;
  using pointer = std::add_pointer_t<input_type>;
  using size_type = typename output_type::size_type;
  constexpr const auto is_view = std::is_same_v<output_type, view_type>;

  auto begin = const_cast<pointer>(&*first);
  (void)begin;
  output_type buffer;
  for (auto it = first; it != last; ++it) {
    if constexpr (is_view) {
      static_assert(
          std::is_same_v<std::random_access_iterator_tag, input_tag>,
          "std::basic_string_view output requires random access input");
      auto end = const_cast<pointer>(&*it);
      auto size = static_cast<size_type>(std::distance(begin, end));
      buffer = view_type{&(*begin), size + 1};
    } else {
      buffer.insert(buffer.end(), *it);
    }
    if (p(buffer)) {
      *d_first++ = buffer;
      if constexpr (is_view) {
        begin = const_cast<pointer>(&*buffer.end());
        buffer = view_type{};
      } else {
        buffer.clear();
      }
    }
  }
  if (!buffer.empty()) {
    *d_first++ = buffer;
  }
  return d_first;
}

}  // namespace chunx
