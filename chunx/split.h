#pragma once
#include <chunx/fixed_length_policy.h>

#include <boost/tokenizer.hpp>

namespace chunx {

template <typename Policy>
using tokenizer_type = boost::tokenizer<Policy&, typename Policy::iterator,
                                        typename Policy::container_type>;

template <typename Policy = fixed_length_policy<64>>
tokenizer_type<Policy> split(typename Policy::iterator first,
                             typename Policy::iterator last,
                             Policy& policy = Policy{});

template <typename Policy>
tokenizer_type<Policy> split(typename Policy::iterator first,
                             typename Policy::iterator last, Policy& policy) {
  return tokenizer_type<Policy>(first, last, policy);
}

}  // namespace chunx
