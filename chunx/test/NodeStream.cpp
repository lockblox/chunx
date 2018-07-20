#include <chunx/InputStreamBuffer.h>
#include <gtest/gtest.h>

namespace chunx
{

struct NodeIO : public ::testing::Test
{
    NodeIO()
        : input(40), chunk_size(10), nodes(std::make_shared<NodeHashMap>()),
          chunker(std::make_shared<RegularChunker>(chunk_size))
    {
        auto node = IndexNode();
        auto root = nodes->insert(node).first;
        node_store = std::make_shared<Array>(nodes.get(), root);
    }

    void SetUp() override
    {

        std::generate(input.begin(), input.end(),
                      [] { return std::rand() % 94 + 32; });
    }
    void TearDown() override
    {
    }

    std::vector<char> input;
    std::size_t chunk_size;
    std::shared_ptr<NodeHashMap> nodes;
    std::shared_ptr<RegularChunker> chunker;
    std::shared_ptr<Array> node_store;
};

TEST_F(NodeIO, Input)
{
    auto chunks = (*chunker)(chunx::string_view(input.data(), input.size()));
    for (auto c : chunks)
    {
        auto node = IndexNode(std::vector<char>(c.begin(), c.end()));
        node_store->insert(node_store->end(), node);
    }

    auto stream = TestNodeStream(node_store, chunker);
    {
        std::istream strbuf(&stream);
        auto output = std::vector<char>{};
        output.resize(input.size());
        strbuf.read(const_cast<char*>(output.data()), output.size());
        EXPECT_EQ(std::size_t(strbuf.gcount()), output.size());
        EXPECT_EQ(std::string(input.begin(), input.end()),
                  std::string(output.begin(), output.end()));
    }
}

TEST_F(NodeIO, Output)
{
    auto stream = TestNodeStream(node_store, chunker);
    {
        // Write test data to the stream
        std::ostream strbuf(&stream);
        strbuf.write(input.data(), input.size());
        strbuf.flush();
        EXPECT_EQ(std::size_t(40), node_store->size());
    }
    {
        // Read test data back from the stream
        std::istream strbuf(&stream);
        auto output = std::vector<char>{};
        output.resize(input.size());
        strbuf.read(const_cast<char*>(output.data()), output.size());
        EXPECT_EQ(std::size_t(strbuf.gcount()), output.size());
        EXPECT_EQ(std::string(input.begin(), input.end()),
                  std::string(output.begin(), output.end()));
    }
}

TEST_F(NodeIO, Seeking)
{
    auto stream = TestNodeStream(node_store, chunker);
    {
        // Write test data to the stream
        auto hsbc = std::string{"Hong Kong Shanhai Banking Corporation"};
        std::ostream strbuf(&stream);
        strbuf.write(hsbc.data(), hsbc.size());
        strbuf.flush();
    }
    {
        // Read test data back from the stream
        std::istream strbuf(&stream);
        strbuf.seekg(0, std::ios_base::beg);
        auto output = std::vector<char>{};
        output.resize(4);
        strbuf.read(const_cast<char*>(output.data()), output.size());
        auto expected = std::string{"Hong"};
        EXPECT_EQ(std::string(expected.begin(), expected.end()),
                  std::string(output.begin(), output.end()));
    }
}

} // namespace chunx
