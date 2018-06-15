#pragma once
#include <cstdio>
#include <cstdlib>
#include <experimental/string_view>
#include <memory>
#include <streambuf>
#include <vector>

namespace chunx
{

// Stream buffers illustrated:

/*------------------------------------------------*\
|           Input Stream Buffer                    |
|                                                  |
|    [x][x][x][x][x][ ][ ][ ][ ][ ][ ]...          |
|     ^              ^                 ^           |
|     |              |                 |           |
|    eback()       gptr()            egptr()       |
|                                                  |
| eback = least recently read byte                 |
| gptr = get pointer; next char to extract         |
| egptr = end get pointer; one past end of buffer  |
| x = bytes which have already been read/extracted |
\*------------------------------------------------*/

/** A streambuf interface for streaming chunked data into/out-of Storage
 */
template <typename InputIterator>
class InputStream : public std::streambuf
{
public:
    /**@brief Create a InputStream
     * @param buffer_size Size of buffer in bytes
     * @param read_size Number of already read bytes to keep */
    explicit InputStream(InputIterator begin,
                         InputIterator end,
                         std::size_t buffer_size = 256,
                         std::size_t read_size = 8);

    InputStream(InputStream&&) = default;
    InputStream& operator=(InputStream&&) = default;
    InputStream(const InputStream&) = delete;
    InputStream& operator=(const InputStream&) = delete;

private:
    int_type underflow() override; /**< Called when more data needed */

    std::streambuf::pos_type seekoff(std::streambuf::off_type off,
                                     std::ios_base::seekdir dir,
                                     std::ios_base::openmode which) override;

    std::streambuf::pos_type seekpos(std::streambuf::pos_type pos,
                                     std::ios_base::openmode which) override;

    using Buffer = typename std::iterator_traits<InputIterator>::value_type;
    using DataIterator = typename Buffer::const_iterator;
    size_t refill_put_back_area();

    /** Read as much data as possible from a Node into the buffer */
    size_t read_data(DataIterator output);

    InputIterator begin_;      /** Beginning of input range */
    InputIterator end_;        /** End of input range */
    InputIterator it_;         /**< Current node being read */
    const size_t read_size_;   /**< Number of already read bytes to keep */
    const size_t buffer_size_; /**< Size of buffer */
    Buffer read_buffer_;       /**< Read buffer for this stream */
    DataIterator read_it_;     /**< Link to current byte */
};

// IMPLEMENTATION

template <typename InputIterator>
InputStream<InputIterator>::InputStream(InputIterator begin,
                                        InputIterator end,
                                        std::size_t buffer_size,
                                        std::size_t read_size)
    : begin_(std::move(begin)), end_(std::move(end)), it_(begin_),
      read_size_(std::max(read_size, size_t(1))), // 1 byte minimum
      buffer_size_(buffer_size)
{
    // buffer is at least as big as read_size_
    read_buffer_.resize(std::max(buffer_size, read_size_) +
                        std::max(read_size, size_t(1)));
    // initialise all pointers to end to indicate that the buffer is empty
    auto read_end = &read_buffer_.front() + read_buffer_.size();
    setg(read_end, read_end, read_end);
}

template <typename InputIterator>
size_t InputStream<InputIterator>::refill_put_back_area()
{
    auto read_begin = read_buffer_.cend();
    std::advance(read_begin, -read_size_); // beginning of region to copy
    std::copy(read_begin, read_buffer_.cend(), read_buffer_.begin());
    return read_size_;
}

template <typename InputIterator>
size_t InputStream<InputIterator>::read_data(
    typename InputStream<InputIterator>::DataIterator output)
{
    auto bytes_left = std::distance(read_it_, it_->cend());
    auto bytes_read = std::distance(output, read_buffer_.cend());
    bytes_read = std::min(bytes_left, bytes_read);
    auto end = read_it_;
    std::advance(end, bytes_read);

    // Note: read_buffer_.erase is a trick to get a non-const iterator
    std::copy(read_it_, end, read_buffer_.erase(output, output));
    std::advance(read_it_, bytes_read);
    return bytes_read;
}

template <typename InputIterator>
std::streambuf::int_type InputStream<InputIterator>::underflow()
{
    if (gptr() < egptr()) // unread data remaining in buffer
    {
        return traits_type::to_int_type(*gptr());
    }
    auto begin = read_buffer_.cbegin();
    auto unread_begin = begin;
    if (eback() == &(*begin)) // eback was reset so at least 1 fill happened
    { // refill put back area and move unread pointer after put back region
        std::advance(unread_begin, refill_put_back_area());
    }
    else
    {
        if (it_ == end_)
        {
            return traits_type::eof();
        }
        else
        {
            it_ = begin_;
            read_it_ = it_->cbegin();
        }
    }

    auto write_iter = unread_begin;
    while (write_iter != read_buffer_.cend())
    {
        if (read_it_ == it_->cend())
        { // Load another
            // block if
            // necessary and
            // one is available
            if (++it_ == end_)
            {
                break;
            }
            else
            {
                read_it_ = it_->cbegin();
            }
        }
        std::advance(write_iter, read_data(write_iter));
    }
    setg(const_cast<char*>(&(*begin)), const_cast<char*>(&(*unread_begin)),
         const_cast<char*>(&(*write_iter))); // Reset buffer pointers
    return traits_type::to_int_type(*gptr());
}

/** Chunking policy
 * Rabin: cut when the fingerprint matches a given value
 * Fixed size: cut when size reaches given value
 * InputIterator takes buffer as input and returns an array of string views */

template <typename InputIterator>
std::streambuf::pos_type
InputStream<InputIterator>::seekoff(std::streambuf::off_type off,
                                    std::ios_base::seekdir dir,
                                    std::ios_base::openmode which)
{
    if (begin_ == end_)
    {
        throw std::out_of_range("seek in empty file");
    }
    if (dir == std::ios_base::cur)
    { // just keep iterators where they are
        if (it_ == end_ and off != 0)
        {
            throw std::out_of_range("seek outside bounds of file");
        }
        printf("seeking from current");
    }
    else if (dir == std::ios_base::beg)
    {
        it_ = begin_;
        read_it_ = it_->cbegin();
        while (off != 0)
        {
            read_it_ = it_->cbegin();
            auto data_remaining = std::distance(read_it_, it_->cend());
            if (off > data_remaining)
            {
                off -= data_remaining;
                ++it_;
                if (it_ == end_)
                {
                    throw std::out_of_range("seek outside bounds of file");
                }
            }
            else
            {
                std::advance(read_it_, off);
                off = 0;
            }
        }
    }
    else if (dir == std::ios_base::end)
    {
        auto node = it_;
        while (node != end_)
        { // advance it_ to penultimate node
            it_ = ++node == end_ ? it_ : node;
        }
        read_it_ = it_->begin();
        auto it = read_it_; // advance read_it_ to penultimate datum
        while (it != it_->end())
        {
            read_it_ = ++it == it_->end() ? read_it_ : it;
        }
    }

    (void)which;
    // readnode_, read_it_ for ios_base::in, ? for ios_base::out
    return 0;
}

template <typename InputIterator>
std::streambuf::pos_type
InputStream<InputIterator>::seekpos(std::streambuf::pos_type pos,
                                    std::ios_base::openmode which)
{
    (void)pos;
    (void)which;
    return 0;
}

} // namespace chunx
