#pragma once
#include <streambuf>
#include <vector>

/*-----------------------------------------------*\
|           Output Stream Buffer                  |
|                                                 |
|    [ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]...         |
|     ^              ^                 ^          |
|     |              |                 |          |
|    pbase()       pptr()            epptr()      |
|                                                 |
| pbase = put base; first element of buf          |
| pptr = put pointer; next location to write      |
| epptr = end put pointer; one past end of buffer |
\*-----------------------------------------------*/

namespace chunx
{

template <typename Chunker, typename OutputIterator>
class OutputStream : public std::streambuf
{
public:
    explicit OutputStream(OutputIterator it,
                          Chunker = Chunker(),
                          std::size_t buffer_size = 256);
    OutputStream(OutputStream&&) = default;
    OutputStream& operator=(OutputStream&&) = default;
    OutputStream(const OutputStream&) = delete;
    OutputStream& operator=(const OutputStream&) = delete;

private:
    int_type overflow(traits_type::int_type ch = traits_type::eof()) override;
    int sync() override;
    OutputIterator it_;
    Chunker chunker_;
    std::vector<char> write_buffer_; /**< Write buffer for this stream */
};

// IMPLEMENTATION
template <typename Chunker, typename OutputIterator>
OutputStream<Chunker, OutputIterator>::OutputStream(OutputIterator it,
                                                    Chunker chunker,
                                                    std::size_t buffer_size)
    : it_(std::move(it)), chunker_(std::move(chunker)),
      write_buffer_(buffer_size)
{
    // Initialise all pointers to end to indicate that the buffer is empty
    auto end = &write_buffer_.front() + write_buffer_.size();
    setp(end, end);
}

template <typename Chunker, typename OutputIterator>
typename OutputStream<Chunker, OutputIterator>::traits_type::int_type
OutputStream<Chunker, OutputIterator>::overflow(traits_type::int_type c)
{                         // wrote to end of buffer so need to transfer
                          // bytes into node and clear
    if (pptr() < epptr()) // space remaining in write buffer
    {
        return traits_type::to_int_type(*pptr());
    }
    if (c == traits_type::eof())
    {
        return traits_type::eof();
    }
    auto begin = write_buffer_.begin();
    if (pbase() != &(*begin))
    { // first call
        auto end = write_buffer_.cend();
        setp(const_cast<char*>(&(*begin)), const_cast<char*>(&(*end)));
    }
    sync();
    *pptr() = c; // write the overflow character to the new write pointer
    pbump(1);    // increment write pointer by 1

    return traits_type::not_eof(c);
}

template <typename Chunker, typename OutputIterator>
int OutputStream<Chunker, OutputIterator>::sync()
{
    auto chunks = chunker_(string_view(pbase(), pptr() - pbase()));
    auto last_begin = pbase();
    auto last_end = pptr();
    for (const auto& c : chunks) // buffer split into chunks
    {
        *it_++ = std::string(c.begin(), c.end());
        last_begin += c.size();
    }
    if (last_begin != last_end)
    {
        *it_++ = std::string(last_begin, last_end);
    }
    auto begin = write_buffer_.begin();
    auto end = write_buffer_.end();
    setp(const_cast<char*>(&(*begin)), const_cast<char*>(&(*end)));
    return 0;
}

} // namespace chunx
