#include <cstring>
#include <iostream>
#include <vector>

class PatriciaTrie {
public:
  struct Node {
    std::vector<Node*> children;
    size_t start;
    size_t len;
    size_t count;

    Node(size_t start, size_t len, size_t count) : start(start), len(len), count(count) {}

    ~Node() { for (auto child : children) delete child; }
  };

  PatriciaTrie(char* data) : root(new Node(0, 0, 0)), data(data), _non_zero_nodes(0) {}

  ~PatriciaTrie() { delete root; }

  void insert(size_t start, size_t end) {
    Node* node = root;
    size_t cur = start;

    while (cur < end) {
      bool cplen_found = false;
      size_t cur_len = end - cur;

      for (auto& child : node->children) {  
        size_t cplen = common_prefix_length(child->start, child->len, cur, cur_len);
        
        if (cplen == 0) {
          continue;
        }
        cplen_found = true;
        
        if (cplen == child->len) {
          if (cplen == cur_len) {
            child->count += 1;
            _non_zero_nodes += child->count == 1;
            return;
          }
          cur += child->len;
          node = child;
          break;
        } else {
          Node* new_node = new Node(child->start + cplen, child->len - cplen, child->count);
          new_node->children = std::move(child->children);
          
          Node* cur_node = new Node(cur + cplen, cur_len - cplen, 1);

          child->len = cplen;
          child->count = 0;
          child->children = { new_node, cur_node };
          return;
        }
      }

      if (!cplen_found) {
        Node* cur_node = new Node(cur, cur_len, 1);
        node->children.push_back(cur_node);
        return;
      }
    }

    throw new std::runtime_error("Impossible");
  }

  size_t remove_all(size_t start, size_t end) {
    return remove_all(root, start, end);
  }

  size_t remove_all(Node* node, size_t start, size_t end) {
    size_t cur_len = end - start;
    for (const auto& child : node->children) {
      size_t cplen = common_prefix_length(child->start, child->len, start, cur_len);
      if (cplen == 0) continue;
      if (cplen == child->len) {
        if (cplen == cur_len) {
          size_t result = child->count;
          _non_zero_nodes -= child->count > 0;
          child->count = 0;
          return result;
        }
        return remove_all(child, start + cplen, end);
      }
      return 0;
    }
    return 0;
  }

  size_t non_zero_nodes() { return _non_zero_nodes; }

private:
  Node* root;
  char* data;
  size_t _non_zero_nodes;

  size_t common_prefix_length(size_t start1, size_t len1, size_t start2, size_t len2) {
    size_t i = 0;
    while (i < len1 && i < len2 && data[start1 + i] == data[start2 + i]) {
      ++i;
    }
    return i;
  }
};
