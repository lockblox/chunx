#include <chunx/Chunker.h>
#include <chunx/NodeStream.h>
#include <map>
#include <objex/HashMap.h>
#include <vertex/Array.h>

namespace chunx
{
// need a codec to convert IndexNode to bytes so it can be hashed by
// shax::HashFunction - indexnodeprotobufcodec
using Map = std::map<shax::Hash, std::vector<char>>;
using NodeMap = std::map<shax::Hash, IndexNode>;
using NodeHashMap = objex::HashMap<std::hash<IndexNode>, NodeMap>;
using Array = objex::Array<NodeHashMap>;

using TestNodeStream = NodeStream<RegularChunker, Array>;
} // namespace chunx
