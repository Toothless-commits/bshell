#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>

class Trie {
public:
    struct Node {
        Node* links[128] = {nullptr};
        bool flag = false;

        ~Node() {
            for (int i = 0; i < 128; i++) {
                delete links[i];
            }
        }

        bool containskey(char ch) {
            unsigned char idx = static_cast<unsigned char>(ch);
            if (idx >= 128) return false;
            return (links[idx] != nullptr);
        }

        void put(char ch, Node* node) {
            unsigned char idx = static_cast<unsigned char>(ch);
            if (idx < 128) {
                links[idx] = node;
            }
        }

        Node* get(char ch) {
            unsigned char idx = static_cast<unsigned char>(ch);
            if (idx >= 128) return nullptr;
            return links[idx];
        }

        void setEnd() {
            flag = true;
        }

        bool isEnd() const {
            return flag;
        }
    };

    Node* root = new Node();

    ~Trie() {
        delete root;
    }

    void insert(const std::string& word) {
        Node* node = root;
        for (char ch : word) {
            if (!node->containskey(ch)) {
                node->put(ch, new Node());
            }
            node = node->get(ch);
            if (!node) return;
        }
        node->setEnd();
    }

    bool search(const std::string& word) {
        Node* node = root;
        for (char ch : word) {
            if (!node->containskey(ch)) return false;
            node = node->get(ch);
            if (!node) return false;
        }
        return node->isEnd();
    }

    bool startswith(const std::string& word) {
        Node* node = root;
        for (char ch : word) {
            if (!node->containskey(ch)) return false;
            node = node->get(ch);
            if (!node) return false;
        }
        return true;
    }

    bool dfs(Node* node, std::string& final_str) {
        if (node->isEnd()) return true;
        for (int i = 0; i < 128; i++) {
            if (node->links[i]) {
                final_str += static_cast<char>(i);
                if (dfs(node->links[i], final_str)) return true;
                final_str.pop_back();
            }
        }
        return false;
    }

    std::string autocomplete(const std::string& cur) {
        Node* node = root;
        std::string final_str = "";
        for (char ch : cur) {
            if (!node->containskey(ch)) return cur;
            final_str += ch;
            node = node->get(ch);
            if (!node) return cur;
        }

        if (dfs(node, final_str)) return final_str;
        return cur;
    }
};

#endif // TRIE_H
