#include <bits/chrono.h>

#include <algorithm>
#include <chrono>
#include <compare>
#include <csignal>
#include <iostream>
#include <memory>
#include <sstream>

#include "SkipList.hpp"
#include "lc.h"
#include "utils.hpp"

#define BIG_SCALE 5000

#define BEGIN_TEST(msg) \
    cout << "========== " << setw(10) << msg << " ==========\n";

#define RUN(X)          \
    do {                \
        BEGIN_TEST(#X); \
        X();            \
        cout << endl;   \
    } while (0);

void lc_test() {
    TEST_N(BIG_SCALE, getRandomDouble());
    TEST_N(BIG_SCALE, getRandomInt());
    TEST(getRandomIntVector());
}

void utils_test() {
    auto data = getRandomIntVector(10000, -50000, 50000);
    const int scale = 1;
#define SORT(X) SORT_TEST(scale, data, sort::X);
    SORT(bubbleSort);
    SORT(seleceSort);
    SORT(quickSort);
}

void list_test() {
    auto list = buildList({1, 2, 3, 4, 5, 6, 7});
    ASSERT_STR_EQUAL("[1,2,3,4,5,6,7]", toString(list));
}

void tree_test() {
    auto tree = new TreeNode(
        1, new TreeNode(2),
        new TreeNode(3, new TreeNode(4, new TreeNode(5), new TreeNode(6))));
    ASSERT_STR_EQUAL("(Pre)[1,2,3,4,5,6],(In)[2,1,5,4,6,3]", toString(tree));
}

void algorithm_test() {
    vector<int> vc{3, 1, 2, 4, 5, 0, 1};
    algorithm::quickSort(vc.begin(), vc.end());
    ASSERT_TRUE(std::is_sorted(vc.begin(), vc.end()));
}

void string_utils_test() {
    auto str = " {This is his preciso s } "s;
    auto splited = splitString(str, "is");
    ASSERT_STR_EQUAL(" {Th, , h, prec,o s } ",
                     trimString(toString(splited), "[]"));
    auto trimed = trimString(str);
    ASSERT_STR_EQUAL("{This is his preciso s }", trimed);
}

void parse_op_test() {
    auto ops = R"(["create","add","sub","mul","add","destory"])";
    auto datas = R"([[],[1],[2],[],[4],[]])";
    auto func = [](string cmd, string data) {
        OUTV("OP: {}, data: {}", cmd, (data == "" ? "null" : data));
    };
    parseOperations(ops, datas, func);
}

void skiplist_test() {
    auto skl = new SkipList<int, string>();
    int scale = 5;
    // insert
    for (int i = 0; i < scale; i++) {
        int k = i;
        string v = getRandomString(1);
        skl->insert(k, v);
    }
    // find
    for (int i = 0; i < scale; i++) {
        ASSERT_TRUE(skl->find(i) != nullptr);
    }
    ASSERT_TRUE(skl->find(scale + 1) == nullptr);
    // erase
    skl->erase(scale - 2);
    ASSERT_TRUE(skl->find(scale - 2) == nullptr);
    // size
    ASSERT_EQUAL(scale - 1, skl->size());

    skl->dump();
}

void skiplist_benchmark() {
    const int length = 100000;
    const int search_times = 1000;

    auto skl = new SkipList<int, int>();
    auto lst = new ListNode(-1);
    auto p = lst;
    map<int, int> mp;
    unordered_map<int, int> ump;
    for (int i = 0; i < length; i++) {
        int r = getRandomInt(0, length);
        skl->insert(r, -1);
        p->next = new ListNode(r);
        p = p->next;
        mp.emplace(r, -1);
        ump.emplace(r, -1);
    }

    // SKIP LIST
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < search_times; i++) {
        skl->find(getRandomInt(0, length));
    }
    auto end = chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    OUTV("[SKIPLIST-Len({})-Search {} Times]: Total {}, {}/per", length,
         search_times, duration,
         chrono::duration_cast<chrono::microseconds>(duration) / search_times);

    // LIST
    start = chrono::steady_clock::now();
    for (int i = 0; i < search_times; i++) {
        int target = getRandomInt(0, length);
        auto p = lst;
        while (p) {
            if (p->val == target) {
                break;
            }
            p = p->next;
        }
    }
    end = chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    OUTV("[LIST-Len({})-Search {} Times]: Total {}, {}/per", length,
         search_times, duration,
         chrono::duration_cast<chrono::microseconds>(duration) / search_times);

    // MAP
    start = chrono::steady_clock::now();
    for (int i = 0; i < search_times; i++) {
        mp.find(getRandomInt(0, length));
    }
    end = chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    OUTV("[MAP-Len({})-Search {} Times]: Total {}, {}/per", length,
         search_times, duration,
         chrono::duration_cast<chrono::microseconds>(duration) / search_times);

    // UNORDERED MAP
    start = chrono::steady_clock::now();
    for (int i = 0; i < search_times; i++) {
        ump.find(getRandomInt(0, length));
    }
    end = chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    OUTV("[UNORDERED_MAP-Len({})-Search {} Times]: Total {}, {}/per", length,
         search_times, duration,
         chrono::duration_cast<chrono::microseconds>(duration) / search_times);
}

void graph_test() {
    auto g = buildRandomGraph(5, false, false, 10, 99);
    print(g);
    auto g2 = Graph<int>({1, 2, 3}, true);
    g2.addNode(5);
    g2.addEdge(1, 2).addEdge(2, 3);
    print(g2);
    auto g3 = buildRandomGraph(50);
    UF uf(g3);
    print(uf);
	uf.normalize();
	print(uf);
}

int main() {
    LOGV("START {} test", "lc");

    // RUN(lc_test);
    RUN(utils_test);
    RUN(list_test);
    RUN(tree_test);
    RUN(algorithm_test);
    RUN(string_utils_test);
    RUN(parse_op_test);
    RUN(skiplist_test);
    // RUN(skiplist_benchmark);
    RUN(graph_test);

    ASSERT_ALL_PASSED();

    cout << endl;
    LOGV("END {} test", "lc");
}
