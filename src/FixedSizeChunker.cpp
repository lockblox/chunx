#include <chunx/FixedSizeChunker.h>
#include <cmath>

namespace chunx
{

FixedSizeChunker::FixedSizeChunker(size_t chunk_size) : chunk_size_(chunk_size)
{
}

size_t FixedSizeChunker::chunk_size() const
{
    return chunk_size_;
}

std::vector<string_view> FixedSizeChunker::operator()(string_view input)
{
    auto input_size = input.size();
    auto total = size_t(floor(float(input_size) / chunk_size_)); // # chunks
    auto output = std::vector<string_view>{};
    output.reserve(total);
    for (auto remaining = total; remaining > 0; --remaining)
    {
        auto offset = chunk_size_ * (total - remaining);
        auto length = std::min(input_size - offset, chunk_size_);
        output.emplace_back(string_view(&input[offset], length));
    }
    return output;
}

} // namespace chunx
