#ifndef LC_RUN_AND_DEBUG_MODE
#define LC_PURE_MODE

static_assert(__cplusplus >= 201703L, "Leetcode OJ platform's cpp version is c++17");

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    explicit ListNode(int v) : val(v), next(nullptr) {
    }

    ListNode(int v, ListNode *n) : val(v), next(n) {
    }

    ~ListNode() {
        delete next;
    }

    string toString();
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int x = 0, TreeNode *left = nullptr, TreeNode *right = nullptr) : val(x), left(left), right(right) {
    }

    ~TreeNode() {
        delete left;
        delete right;
    }

    string toString();
};

#endif