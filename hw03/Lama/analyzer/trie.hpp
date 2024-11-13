#include <vector>
#include <memory>
#include <cstring>

static const int LEVEL_SIZE = 256;

struct ByteRange {
  std::unique_ptr<char[]> start;
  char len;
  int frequency;

  ByteRange(const std::vector<char>& path, int _frequency) : len((char)path.size()), frequency(_frequency) {
    start = std::make_unique<char[]>(len);
    std::memcpy(start.get(), path.data(), path.size());
  }

  ByteRange(ByteRange&& other) : start(std::move(other.start)), len(other.len), frequency(other.frequency) {}

  ByteRange& operator=(ByteRange&& other) {
    if (this != &other) {
      start = std::move(other.start);
      len = other.len;
      frequency = other.frequency;
    }
    return *this;
  }

  ByteRange(const ByteRange&) = delete;
  ByteRange& operator=(const ByteRange&) = delete;
};

struct TrieNode {
  int count;
  TrieNode **children;

  TrieNode() : count(0), children(nullptr) {}

  ~TrieNode() {
    if (!children) return;
    for (int i = 0; i < LEVEL_SIZE; ++i) {
      delete children[i];
    }
    delete[] children;
  }

  void init_children() {
    if (children) return;
    children = new TrieNode*[LEVEL_SIZE]();
    for (int i = 0; i < LEVEL_SIZE; ++i) {
      children[i] = nullptr;
    }
  }
};

class Trie {
public:
  Trie() : root(new TrieNode()), non_zero_nodes(0) {}

  ~Trie() {
    delete root;
  }

  void insert(char *start, char *end) {
    TrieNode *node = root;
    for (char *it = start; it != end; ++it) {
      uint8_t index = (uint8_t)(*it);
      if (!node->children) {
        node->init_children();
      }
      if (!node->children[index]) {
        node->children[index] = new TrieNode();
      }
      node = node->children[index];
    }
    if (node->count == 0) non_zero_nodes++;
    node->count++;
  }

  std::vector<ByteRange> collect() {
    std::vector<char> path;
    std::vector<ByteRange> bytecodes;
    bytecodes.reserve(non_zero_nodes);
    collect(root, path, bytecodes);
    return bytecodes;
  }

private:
  TrieNode *root;
  int non_zero_nodes;

  void collect(TrieNode *node, std::vector<char> &path, std::vector<ByteRange> &bytecodes) {
    if (node->count > 0) {
      bytecodes.emplace_back(path, node->count);
    }
    
    if (!node->children) return;

    for (int i = 0; i < LEVEL_SIZE; ++i) {
      if (!node->children[i]) continue;
      path.push_back((char)i);
      collect(node->children[i], path, bytecodes);
      path.pop_back();
    }
  }
};
