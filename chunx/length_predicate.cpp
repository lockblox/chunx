#include <chunx/length_predicate.h>

#include <cmath>

namespace chunx {

length_predicate::length_predicate(size_t chunk_size) : size_(chunk_size) {}

std::size_t length_predicate::length() const { return size_; }

length_predicate& length_predicate::length(std::size_t value) {
  size_ = value;
  return *this;
}

}  // namespace chunx
