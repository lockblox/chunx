#include <chunx/InputStream.h>
#include <gtest/gtest.h>
#include <iterator>

namespace chunx
{

TEST(Chunx, InputStream)
{
    using Container = std::set<std::string>;
    using Iterator = Container::const_iterator;
    using Stream = InputStream<Iterator>;
    auto container = Container{"abcd", "efgh", "ijkl", "m"};
    auto stream = Stream(container.cbegin(), container.cend());
    std::istream strbuf(&stream);
    auto expected = std::string{"abcdefghijklm"};
    auto output = std::string{};
    output.resize(expected.size());
    strbuf.read(const_cast<char*>(output.data()), output.size());
    EXPECT_EQ(expected, output);
}

} // namespace chunx
