#include <chunx/FixedSizeChunker.h>
#include <chunx/OutputStream.h>
#include <gtest/gtest.h>
#include <iterator>

namespace chunx
{

TEST(Chunx, OutputStream)
{
    using Container = std::set<std::string>;
    using Chunker = FixedSizeChunker;
    using Inserter = std::insert_iterator<Container>;
    using Stream = OutputStream<Chunker, Inserter>;
    auto chunker = Chunker(4);
    auto container = Container{};
    auto inserter = Inserter(container, container.end());
    auto stream = Stream(inserter, chunker);
    std::ostream strbuf(&stream);
    strbuf << "abcdefghijklm";
    strbuf.flush();
    auto expected = Container{"abcd", "efgh", "ijkl", "m"};
    EXPECT_EQ(expected, container);
}

} // namespace chunx
