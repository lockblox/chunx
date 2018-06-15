#include <algorithm>
#include <chunx/FixedSizeChunker.h>
#include <gtest/gtest.h>

TEST(Chunker, FixedSizeChunker)
{
    using string_view = std::experimental::fundamentals_v1::string_view;
    auto input = std::string("123456789");
    {
        auto chunker = chunx::FixedSizeChunker(2);
        auto chunk_refs = chunker(input);
        auto results = std::vector<std::string>{};
        std::transform(chunk_refs.begin(), chunk_refs.end(),
                       std::back_inserter(results),
                       [](string_view ref) { return std::string(ref); });
        auto expected = std::vector<std::string>{"12", "34", "56", "78"};
        EXPECT_EQ(expected, results);
    }

    {
        auto chunker = chunx::FixedSizeChunker(3);
        auto chunk_refs = chunker(input);
        auto results = std::vector<std::string>{};
        std::transform(chunk_refs.begin(), chunk_refs.end(),
                       std::back_inserter(results),
                       [](string_view ref) { return std::string(ref); });
        auto expected = std::vector<std::string>{"123", "456", "789"};
        EXPECT_EQ(expected, results);
    }
    {
        input = std::string("1");
        auto chunker = chunx::FixedSizeChunker(3);
        auto chunk_refs = chunker(input);
        auto results = std::vector<std::string>{};
        std::transform(chunk_refs.begin(), chunk_refs.end(),
                       std::back_inserter(results),
                       [](string_view ref) { return std::string(ref); });
        auto expected = std::vector<std::string>{};
        EXPECT_EQ(expected, results);
    }
}
