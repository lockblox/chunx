#pragma once
#include <string>

namespace chunx {

// TODO add a code to select the splitting policy

template <typename InputIt = std::string::iterator,
          typename Container = std::string>
class policy {
 public:
  using container_type = Container;
  using iterator = InputIt;
  virtual ~policy() {}
  virtual bool operator()(InputIt& first, InputIt last, Container& token) = 0;
  virtual void reset() {}
};

}  // namespace chunx
