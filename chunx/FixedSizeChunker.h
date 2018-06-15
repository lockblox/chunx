#pragma once
#include <experimental/string_view>
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <vector>

namespace chunx
{

using string_view = boost::string_ref;

class FixedSizeChunker
{
public:
    explicit FixedSizeChunker(size_t chunk_size = 262144);
    ~FixedSizeChunker() = default;

    size_t chunk_size() const;

    /**@brief Process all characters in string_view and return any chunks */
    std::vector<string_view> operator()(string_view input);

private:
    size_t chunk_size_;
};

} // namespace chunx
