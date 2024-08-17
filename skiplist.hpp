#ifndef SHUAIKAI_SKIP_LIST_H
#define SHUAIKAI_SKIP_LIST_H

#include <math.h>

#include "lc.h"

template <typename K, typename V>
struct SkipListNode {
    K key;
    V val;

    int level;
    std::vector<SkipListNode*> nextNodes;

    SkipListNode(int level, K k = K{}, V v = V{})
        : key(k), val(v), level(level), nextNodes(vector<SkipListNode*>(level, nullptr)) {
    }
};

template <typename K, typename V>
class SkipList {
public:
    SkipList() : node_num(0), head(new SkipListNode<K, V>(max_level)) {
    }

    SkipListNode<K, V>* find(K key) const;
    V& operator[](K key);
    bool insert(K key, V val);
    bool erase(K key);

    int size() const {
        return node_num;
    }

    bool empty() const {
        return node_num == 0;
    }

    void dump() const;

private:
    static int get_random_level();

    static int max_level;
    int node_num;
    SkipListNode<K, V>* head;
};

template <typename K, typename V>
int SkipList<K, V>::max_level = 20;

template <typename K, typename V>
int SkipList<K, V>::get_random_level() {
    auto engine = std::mt19937{std::random_device{}()};
    auto distro = std::uniform_real_distribution<float>(0, 1);
    int level = 1;
    while (distro(engine) < 0.5 && level <= max_level) {
        ++level;
    }
    return level;
}

template <typename K, typename V>
SkipListNode<K, V>* SkipList<K, V>::find(K key) const {
    auto curr = head;
    for (int i = max_level - 1; i >= 0; --i) {
        while (curr->nextNodes[i] != nullptr && curr->nextNodes[i]->key < key) {
            curr = curr->nextNodes[i];
        }
    }
    curr = curr->nextNodes[0];
    if (curr == nullptr || curr->key != key) {
        return nullptr;
    }
    return curr;
}

template <typename K, typename V>
V& SkipList<K, V>::operator[](K key) {
    auto r = this->find(key);
    if (r == nullptr) {
        throw std::runtime_error("Unexists Key");
    }
    return r->val;
}

template <typename K, typename V>
bool SkipList<K, V>::insert(K key, V val) {
    auto curr = head;
    std::vector<SkipListNode<K, V>*> update(max_level, nullptr);
    for (int i = max_level - 1; i >= 0; --i) {
        while (curr->nextNodes[i] != nullptr && curr->nextNodes[i]->key < key) {
            curr = curr->nextNodes[i];
        }
        update[i] = curr;
    }
    curr = curr->nextNodes[0];
    if (curr != nullptr && curr->key == key) {
        return false;
    }
    int level = get_random_level();
    auto newNode = new SkipListNode<K, V>(level, key, val);
    for (int i = 0; i < level; ++i) {
        newNode->nextNodes[i] = update[i]->nextNodes[i];
        update[i]->nextNodes[i] = newNode;
    }
    ++node_num;
    return true;
}

template <typename K, typename V>
bool SkipList<K, V>::erase(K key) {
    if (node_num <= 0) {
        return false;
    }
    auto curr = head;
    std::vector<SkipListNode<K, V>*> update(max_level, nullptr);
    for (int i = max_level - 1; i >= 0; --i) {
        while (curr->nextNodes[i] != nullptr && curr->nextNodes[i]->key < key) {
            curr = curr->nextNodes[i];
        }
        update[i] = curr;
    }
    curr = curr->nextNodes[0];
    if (curr == nullptr || curr->key != key) {
        return false;
    }
    int level = curr->level;
    for (int i = 0; i < level; ++i) {
        update[i]->nextNodes[i] = curr->nextNodes[i];
        curr->nextNodes[i] = nullptr;
    }
    delete curr;
    --node_num;
    return true;
}

template <typename K, typename V>
void SkipList<K, V>::dump() const {
    for (int i = 0; i < max_level; ++i) {
        auto p = head->nextNodes[i];
        if (p == nullptr) {
            continue;
        }
        std::cout << formatStr("[LEVEL-{}]: (HEAD)->", i + 1);
        while (p != nullptr) {
            std::cout << formatStr("({},{},[{}])->", p->key, p->val, p->level);
            p = p->nextNodes[i];
        }
        cout << "(NULL)\n\n";
    }
}

#endif  // SHUAIKAI_SKIP_LIST_H
