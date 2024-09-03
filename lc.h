#ifndef SHUAIKAI_LC_H
#define SHUAIKAI_LC_H

static_assert(__cplusplus >= 202002L, "Keep up with the times and embrace C++20, young people!");

#ifdef __linux__
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#endif

// Since Boost is present on my Linux system but not on Windows, I'll use __linux__ for the check.
#ifdef __linux__
#include <boost/version.hpp>
#endif

#if BOOST_VERSION >= 107400
#include <boost/type_index.hpp>
#endif

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class SpinLock {
private:
    std::atomic_flag flag;

public:
    SpinLock() : flag(ATOMIC_FLAG_INIT) {
    }

    SpinLock(const SpinLock &) = delete;
    SpinLock operator=(const SpinLock &) = delete;

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
        }
    }

    void unlock() {
        flag.clear();
    }
};

class SpinLockGuard {
private:
    SpinLock &lock;

public:
    explicit SpinLockGuard(SpinLock &lck) : lock(lck) {
        lock.lock();
    }

    ~SpinLockGuard() {
        lock.unlock();
    }

    SpinLockGuard(const SpinLockGuard &) = delete;
    SpinLockGuard operator=(const SpinLockGuard &) = delete;
};

SpinLock globalLogSpinLock;  // global cout lock
#define GUARD_LOG SpinLockGuard guard(globalLogSpinLock)

// ERRNO
#define EXIT_ASSERT_FAIL   900
#define EXIT_PARAM_ILLEGAL 901
#define EXIT_CTRL_C        902

#define THREAD_SAFE_EXIT(x) exit(x)

/// MARK: common MCRO

#define ANSI_CLEAR     "\033[0m"
#define ANSI_RED_BG    "\033[0;31m"
#define ANSI_GREEN_BG  "\033[0;32m"
#define ANSI_YELLOW_BG "\033[0;33m"
#define ANSI_BLUE_BG   "\033[0;34m"
#define ANSI_PURPLE_BG "\033[0;35m"

#define ELEM_SEP ","

#define COUT(x)                                                                      \
    do {                                                                             \
        std::string ret = toString(x);                                               \
        GUARD_LOG;                                                                   \
        std::cout << ANSI_BLUE_BG << "[" #x "]: " << ANSI_CLEAR << ret << std::endl; \
    } while (0);

#define COUT_POSITION "[" << __FILE__ << ELEM_SEP << __FUNCTION__ << ELEM_SEP << __LINE__ << "]"

#define TODO(msg)                                                                                           \
    do {                                                                                                    \
        GUARD_LOG;                                                                                          \
        std::cerr << ANSI_YELLOW_BG << (msg) << ANSI_PURPLE_BG << COUT_POSITION << ANSI_CLEAR << std::endl; \
    } while (0);

#define FILL_ME() TODO("Fill Code Here!!! -> ")

#define LINE_BREAKER(msg)                                                                                  \
    do {                                                                                                   \
        GUARD_LOG;                                                                                         \
        std::cout << ANSI_YELLOW_BG << "========== " << (msg) << " ==========" << ANSI_CLEAR << std::endl; \
    } while (0);

#define NEW_LINE()              \
    do {                        \
        GUARD_LOG;              \
        std::cout << std::endl; \
    } while (0);

/// MARK: test MCRO

std::atomic<int> gFailedTest(0);  // NOLINT
std::atomic<int> gTotalTest(0);   // NOLINT

#define ASSERT_MSG(expr, msg)                                             \
    do {                                                                  \
        if (!(expr)) {                                                    \
            GUARD_LOG;                                                    \
            std::cerr << ANSI_RED_BG << (msg) << ANSI_CLEAR << std::endl; \
            THREAD_SAFE_EXIT(EXIT_ASSERT_FAIL);                           \
        }                                                                 \
    } while (0)

#define ASSERT(expr)                                                                                      \
    do {                                                                                                  \
        if (!(expr)) {                                                                                    \
            GUARD_LOG;                                                                                    \
            std::cerr << ANSI_RED_BG << "Assert " << ANSI_PURPLE_BG << #expr << ANSI_RED_BG << " Failed!" \
                      << ANSI_CLEAR << std::endl;                                                         \
            THREAD_SAFE_EXIT(EXIT_ASSERT_FAIL);                                                           \
        }                                                                                                 \
    } while (0)

#define ASSERT_TRUE(expr)                                                                                             \
    do {                                                                                                              \
        if (expr) {                                                                                                   \
            GUARD_LOG;                                                                                                \
            std::cout << ANSI_GREEN_BG << "[PASSED] " << ANSI_CLEAR << "[" + std::to_string(gTotalTest.load()) + "] " \
                      << ANSI_BLUE_BG << #expr << ANSI_GREEN_BG << " => true" << ANSI_CLEAR << std::endl;             \
        } else {                                                                                                      \
            ++gFailedTest;                                                                                            \
            GUARD_LOG;                                                                                                \
            std::cerr << ANSI_RED_BG << "[FAILED] " << ANSI_CLEAR << "[" + std::to_string(gTotalTest.load()) + "] "   \
                      << ANSI_BLUE_BG << #expr << ANSI_RED_BG << " => istrue? " << ANSI_YELLOW_BG << COUT_POSITION    \
                      << ANSI_CLEAR << std::endl;                                                                     \
            std::cerr << ANSI_PURPLE_BG << '\t' << "Expected: " << ANSI_CLEAR << "True" << std::endl;                 \
            std::cerr << ANSI_PURPLE_BG << '\t' << "  Actual: " << ANSI_CLEAR << "False" << std::endl;                \
        }                                                                                                             \
        ++gTotalTest;                                                                                                 \
    } while (0)

/// x: expected, y: acutal
#define ASSERT_EQUAL(x, y)                                                                                            \
    do {                                                                                                              \
        if ((x) == (y)) {                                                                                             \
            GUARD_LOG;                                                                                                \
            std::cout << ANSI_GREEN_BG << "[PASSED] " << ANSI_CLEAR << "[" + std::to_string(gTotalTest.load()) + "] " \
                      << ANSI_BLUE_BG << #x << ANSI_GREEN_BG << " == " << ANSI_BLUE_BG << #y << ANSI_CLEAR            \
                      << std::endl;                                                                                   \
        } else {                                                                                                      \
            ++gFailedTest;                                                                                            \
            GUARD_LOG;                                                                                                \
            std::cerr << ANSI_RED_BG << "[FAILED] " << ANSI_CLEAR << "[" + std::to_string(gTotalTest.load()) + "] "   \
                      << ANSI_BLUE_BG << #x << ANSI_RED_BG " == " << ANSI_BLUE_BG << #y " " << ANSI_YELLOW_BG         \
                      << COUT_POSITION << ANSI_CLEAR << std::endl;                                                    \
            std::cerr << ANSI_PURPLE_BG << '\t' << "Expected: " << ANSI_CLEAR << "Equal" << std::endl;                \
            std::cerr << ANSI_PURPLE_BG << '\t' << "  Actual: " << ANSI_CLEAR << "NonEqual" << std::endl;             \
        }                                                                                                             \
        ++gTotalTest;                                                                                                 \
    } while (0)

/// x: expected, y: acutal
#define STR_EQUAL_(x, y)                                                                                              \
    do {                                                                                                              \
        auto expected = toString(x);                                                                                  \
        auto actual = toString(y);                                                                                    \
        if (expected == actual) {                                                                                     \
            GUARD_LOG;                                                                                                \
            std::cout << ANSI_GREEN_BG << "[PASSED] " << ANSI_CLEAR << "[" + std::to_string(gTotalTest.load()) + "] " \
                      << ANSI_BLUE_BG << #x << ANSI_GREEN_BG << " == " << ANSI_BLUE_BG << #y << ANSI_CLEAR            \
                      << std::endl;                                                                                   \
        } else {                                                                                                      \
            ++gFailedTest;                                                                                            \
            GUARD_LOG;                                                                                                \
            std::cerr << ANSI_RED_BG << "[FAILED] " << ANSI_CLEAR << "[" + std::to_string(gTotalTest.load()) + "] "   \
                      << ANSI_BLUE_BG << #x << ANSI_RED_BG << " == " << ANSI_BLUE_BG << #y " " << ANSI_YELLOW_BG      \
                      << COUT_POSITION << ANSI_CLEAR << std::endl;                                                    \
            std::cerr << ANSI_PURPLE_BG << '\t' << "Expected: " << ANSI_CLEAR << expected << std::endl;               \
            std::cerr << ANSI_PURPLE_BG << '\t' << "  Actual: " << ANSI_CLEAR << actual << std::endl;                 \
        }                                                                                                             \
        ++gTotalTest;                                                                                                 \
    } while (0)

#define ASSERT_STR_EQUAL(x, y) STR_EQUAL_(x, y)

#define ASSERT_ALL_PASSED()                                                                           \
    do {                                                                                              \
        GUARD_LOG;                                                                                    \
        std::cout << std::endl;                                                                       \
        if (gFailedTest.load() == 0) {                                                                \
            std::cout << ANSI_GREEN_BG << "==== "                                                     \
                      << std::to_string(gTotalTest.load()) + "/" + std::to_string(gTotalTest.load())  \
                             + " PASSED ALL! ===="                                                    \
                      << ANSI_CLEAR << std::endl;                                                     \
        } else {                                                                                      \
            std::cout << ANSI_RED_BG << "==== "                                                       \
                      << std::to_string(gFailedTest.load()) + "/" + std::to_string(gTotalTest.load()) \
                             + " test failed! ===="                                                   \
                      << ANSI_CLEAR << std::endl;                                                     \
        }                                                                                             \
    } while (0)

// 让X语句循环n次
#define LOOP_N(n, X)        \
    do {                    \
        int cnt = n;        \
        while (cnt-- > 0) { \
            X;              \
        }                   \
    } while (0);

// 统计 X 执行单次耗时
#define TIME(msg, X)                                                                                  \
    do {                                                                                              \
        auto start = std::chrono::steady_clock::now();                                                \
        X;                                                                                            \
        auto end = std::chrono::steady_clock::now();                                                  \
        auto miliduring = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
        GUARD_LOG;                                                                                    \
        std::cout << "[" << (msg) << "]: " << miliduring << "ms\n";                                   \
    } while (0);

// 让X执行n次，统计耗时与平均耗时
#define TIME_N(msg, n, X)                                                                                              \
    do {                                                                                                               \
        int cnt = n;                                                                                                   \
        auto start = std::chrono::steady_clock::now();                                                                 \
        while (cnt-- > 0) {                                                                                            \
            X;                                                                                                         \
        }                                                                                                              \
        auto end = std::chrono::steady_clock::now();                                                                   \
        auto cost = end - start;                                                                                       \
        auto miliduring = std::chrono::duration_cast<std::chrono::milliseconds>(cost).count();                         \
        auto microduring = std::chrono::duration_cast<std::chrono::microseconds>(cost).count();                        \
        GUARD_LOG;                                                                                                     \
        std::cout << "[" << (n) << " " << (msg) << "]: " << miliduring << "ms, (" << microduring / (n) << "us/per)\n"; \
    } while (0);

// 测试X执行n次耗时。对msg进行了包装，直接传入X的名称
#define SCALE            1000
#define TEST_N(scale, X) TIME_N(#X, scale, X)
#define TEST(X)          TIME_N(#X, SCALE, X)

/// MARK: printer
struct ListNode;
struct TreeNode;

template <typename T>
concept LeetcodePointerType = std::is_same_v<ListNode *, T> || std::is_same_v<TreeNode *, T>;

template <typename T>
concept StreamOutable = requires(std::ostream &os, T elem) {
    { os << elem } -> std::same_as<std::ostream &>;
};

template <typename T>
concept Serializable = requires(T obj) {
    { obj.toString() } -> std::convertible_to<std::string_view>;
};

template <typename T>
concept SequentialContainer = requires(T c) {
    typename T::value_type;
    { c.cbegin() } -> std::same_as<typename T::const_iterator>;
    { c.cend() } -> std::same_as<typename T::const_iterator>;
};

template <typename T>
concept MappedContainer = requires(T m) {
    typename T::key_type;
    typename T::mapped_type;
    { m.cbegin() } -> std::same_as<typename T::const_iterator>;
    { m.cend() } -> std::same_as<typename T::const_iterator>;
};

template <typename T>
concept StackLike = requires(T m) {
    typename T::value_type;
    { m.pop() } -> std::same_as<void>;
    { m.top() } -> std::convertible_to<typename T::const_reference>;
    { m.empty() } -> std::same_as<bool>;
};

template <typename T>
concept QueueLike = requires(T m) {
    typename T::value_type;
    { m.pop() } -> std::same_as<void>;
    { m.front() } -> std::convertible_to<typename T::const_reference>;
    { m.empty() } -> std::same_as<bool>;
};

template <typename T>
concept PairLike = requires(T p) {
    { std::get<0>(p) } -> std::convertible_to<typename T::first_type>;
    { std::get<1>(p) } -> std::convertible_to<typename T::second_type>;
};

template <typename T>
concept Printable = StreamOutable<T> || Serializable<T> || SequentialContainer<T> || MappedContainer<T> || PairLike<
    T> || StackLike<T> || QueueLike<T>;

template <bool>
auto toString(bool obj);

template <Printable T>
auto toString(const T &obj);

template <typename T>
requires Printable<typename T::value_type>
auto forBasedContainer2String(const T &c);

template <SequentialContainer T>
requires Printable<typename T::value_type>
auto SequentialContainer2String(const T &c);

template <PairLike T>
requires Printable<typename T::first_type> && Printable<typename T::second_type>
auto Pair2String(const T &p);

template <MappedContainer T>
requires Printable<typename T::key_type> && Printable<typename T::mapped_type>
auto MappedContainer2String(const T &c);

template <StackLike T>
requires Printable<typename T::value_type>
auto Stack2String(const T &c);

template <QueueLike T>
requires Printable<typename T::value_type>
auto Queue2String(const T &c);

/// MARK: LEETCODE

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

/// MARK: tree/List tools
inline string treeToString(TreeNode *root, string sep) {
    string ret;
    // inorder
    stringstream ss;
    TreeNode *p = root;
    ss << "(In)[";
    std::function<void(TreeNode *)> inorder = [&](TreeNode *node) {
        if (node != nullptr) {
            inorder(node->left);
            ss << node->val << sep;
            inorder(node->right);
        }
    };
    inorder(p);
    ret = ss.str().substr(0, ss.str().length() - sep.length()) + "]";
    // preorder
    stringstream ss2;
    ss2 << "(Pre)[";
    std::function<void(TreeNode *)> preorder = [&](TreeNode *node) {
        if (node != nullptr) {
            ss2 << node->val << sep;
            preorder(node->left);
            preorder(node->right);
        }
    };
    TreeNode *q = root;
    preorder(q);
    string ret2 = ss2.str().substr(0, ss2.str().length() - sep.length()) + "]";
    return ret2 + sep + ret;
}

inline string listToString(ListNode *list, const string &sep) {
    stringstream ss;
    ListNode *p = list;
    ss << "[";
    while (p != nullptr) {
        ss << p->val << sep;
        p = p->next;
    }
    return ss.str().substr(0, ss.str().length() - sep.length()) + "]";
}

inline string TreeNode::toString() {
    return treeToString(this, ELEM_SEP);
}

inline string ListNode::toString() {
    return listToString(this, ELEM_SEP);
}

inline TreeNode *buildTree(span<int> preorder, span<int> inorder) {
    auto *root = new TreeNode(preorder[0]);
    int len = preorder.size();
    if (len == 1) {
        return root;
    }
    int idx = -1;
    for (int i = 0; i < len; i++) {
        if (inorder[i] == preorder[0]) {
            idx = i;
        }
    }
    root->left = buildTree(preorder.subspan(1, idx), inorder.subspan(0, idx));
    root->right = buildTree(preorder.subspan(idx + 1, len - idx - 1), inorder.subspan(idx + 1, len - idx - 1));
    return root;
}

inline TreeNode *buildRandomTree(int len = 10) {
    FILL_ME();
    return nullptr;
}

inline ListNode *vector2List(vector<int> &&data) {
    int len = data.size();
    auto *head = new ListNode(data[0]);
    auto *tail = head;
    for (int i = 1; i < len; i++) {
        tail->next = new ListNode(data[i]);
        tail = tail->next;
    }
    return head;
}

inline ListNode *buildList(vector<int> &data) {
    return vector2List(std::move(data));
}

inline void reverseList(ListNode **head) {
    ListNode *p = *head;
    ListNode *q = p->next;
    if (q == nullptr) {
        return;
    }
    p->next = nullptr;
    while (q->next != nullptr) {
        ListNode *r = q->next;
        q->next = p;
        p = q;
        q = r;
    }
    q->next = p;
    *head = q;
}

/// MARK: printer Impl

template <PairLike T>
requires Printable<typename T::first_type> && Printable<typename T::second_type>

auto Pair2String(const T &p) {
    std::stringstream ss;
    ss << '{' << toString(std::get<0>(p)) << ELEM_SEP << toString(std::get<1>(p)) << '}';
    return ss.str();
}

template <typename T>
requires Printable<typename T::value_type>

auto forBasedContainer2String(const T &c) {
    if (c.empty()) {
        return "[]"s;
    }
    // 会有类型问题？
    // std::accumulate(std::next(c.begin()), c.end(),
    // toString(*(c.begin())),
    // [](string a, auto b){return a + ELEM_SEP + toString(b);});

    std::stringstream ss;
    ss << "[";
    for (const auto &elem : c) {
        ss << toString(elem) << ELEM_SEP;
    }
    string ret = ss.str();
    for (int i = 0; i < strlen(ELEM_SEP); ++i) {
        ret.pop_back();
    }
    ret.push_back(']');
    return ret;
}

template <SequentialContainer T>
requires Printable<typename T::value_type>

auto SequentialContainer2String(const T &c) {
    return forBasedContainer2String(c);
}

template <MappedContainer T>
requires Printable<typename T::key_type> && Printable<typename T::mapped_type>

auto MappedContainer2String(const T &c) {
    return forBasedContainer2String(c);
}

template <StackLike T>
requires Printable<typename T::value_type>

auto Stack2String(const T &c) {
    if (c.empty()) {
        return "[]"s;
    }
    const string stack_sep = "<-";
    T tmp = c;
    stringstream ss;
    ss << "Stack[" << toString(tmp.top()) << stack_sep;
    tmp.pop();
    while (!tmp.empty()) {
        ss << toString(tmp.top()) << stack_sep;
        tmp.pop();
    }
    string ret = ss.str();
    for (int i = 0; i < stack_sep.length(); ++i) {
        ret.pop_back();
    }
    ret.append("]");
    return ret;
}

template <QueueLike T>
requires Printable<typename T::value_type>

auto Queue2String(const T &c) {
    if (c.empty()) {
        return "[]"s;
    }
    const string queue_sep = "<-";
    T tmp = c;
    stringstream ss;
    ss << "Queue[" << toString(tmp.front()) << queue_sep;
    tmp.pop();
    while (!tmp.empty()) {
        ss << toString(tmp.front()) << queue_sep;
        tmp.pop();
    }
    string ret = ss.str();
    for (int i = 0; i < queue_sep.length(); ++i) {
        ret.pop_back();
    }
    ret.append("]");
    return ret;
}

template <Printable T>
auto toString(const T &obj) {
    // 优先使用自带的 toString() 方法
    if constexpr (LeetcodePointerType<T>) {
        return obj->toString();
    } else if constexpr (Serializable<T>) {
        return obj.toString();
    } else if constexpr (StreamOutable<T>) {
        std::stringstream ss;
        ss << obj;
        return std::move(ss.str());
    } else if constexpr (SequentialContainer<T>) {
        return SequentialContainer2String(obj);
    } else if constexpr (MappedContainer<T>) {
        return MappedContainer2String(obj);
    } else if constexpr (PairLike<T>) {
        return Pair2String(obj);
    } else if constexpr (StackLike<T>) {
        return Stack2String(obj);
    } else if constexpr (QueueLike<T>) {
        return Queue2String(obj);
    } else {
        GUARD_LOG;
        std::cerr << ANSI_RED_BG << "Isn't Printable\n" << ANSI_CLEAR;
        return "@@FALSE_STRING@@";
    }
}

template <bool>
auto toString(bool obj) {
    return obj ? "Ture" : "False";
}

template <Printable T>
void print(const T &obj, const std::string &prefix = "", const std::string &suffix = "", bool lineBreak = true) {
    std::cout << prefix << toString(obj) << suffix;
    if (lineBreak) {
        std::cout << "\n";
    }
}

template <Printable... Args>
void dump(Args... args) {
    ((std::cout << toString(args) << " "), ...);
    std::cout << "\n";
}

template <PairLike... PairType>
void dumpWithName(PairType... args) {
    GUARD_LOG;
    ((std::cout << ANSI_BLUE_BG << "[" << toString(std::get<0>(args)) << "]:" << ANSI_CLEAR
                << toString(std::get<1>(args)) << " "),
     ...);
}

template <PairLike... PairType>
void listWithName(PairType... args) {
    GUARD_LOG;
    ((std::cout << ANSI_BLUE_BG << "[" << toString(std::get<0>(args)) << "]:" << ANSI_CLEAR
                << toString(std::get<1>(args)) << "\n"),
     ...);
}

/// MARK: log

#define LOGV(...)                                                         \
    do {                                                                  \
        GUARD_LOG;                                                        \
        std::cout << ANSI_BLUE_BG << COUT_POSITION << ": " << ANSI_CLEAR; \
        printFormatedString(__VA_ARGS__);                                 \
    } while (0);
#define LOG(X) LOGV("{}", X);

#define TO_PAIR(x)    std::make_pair(#x, x)
#define DUMP1(x)      dumpWithName(TO_PAIR(x))
#define DUMP2(x, ...) dumpWithName(TO_PAIR(x)), DUMP1(__VA_ARGS__)
#define DUMP3(x, ...) dumpWithName(TO_PAIR(x)), DUMP2(__VA_ARGS__)
#define DUMP4(x, ...) dumpWithName(TO_PAIR(x)), DUMP3(__VA_ARGS__)
#define DUMP5(x, ...) dumpWithName(TO_PAIR(x)), DUMP4(__VA_ARGS__)
#define DUMP6(x, ...) dumpWithName(TO_PAIR(x)), DUMP5(__VA_ARGS__)
#define DUMP7(x, ...) dumpWithName(TO_PAIR(x)), DUMP6(__VA_ARGS__)
#define DUMP8(x, ...) dumpWithName(TO_PAIR(x)), DUMP7(__VA_ARGS__)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME

// 将每一个变量都按照[name]:value的格式打印出来
#define DUMP(...)                                                                      \
    do {                                                                               \
        GET_MACRO(__VA_ARGS__, DUMP8, DUMP7, DUMP6, DUMP5, DUMP4, DUMP3, DUMP2, DUMP1) \
        (__VA_ARGS__);                                                                 \
        std::cout << "\n";                                                             \
    } while (0);
#define OUT(...) DUMP(__VA_ARGS__)
// OUTV是格式化打印，区分DUMP,后者是将所有的都打印出名字
#define OUTV(...)                         \
    do {                                  \
        GUARD_LOG;                        \
        printFormatedString(__VA_ARGS__); \
    } while (0);

int constexpr countSubStr(std::string_view str, std::string_view substr) {
    int cnt = 0;
    size_t pos = 0;
    auto sublength = substr.length();
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        cnt++;
        pos += sublength;
    }
    return cnt;
}

template <typename... Args>
std::string formatStr(std::string_view format, const Args &...args) {
    int placeholdersCount = countSubStr(format, "{}");
    ASSERT_MSG((placeholdersCount == sizeof...(args)),
               "Failed to call LOGV, because num of {} must equal to args you "
               "passed! "
                   + formatStr("Expected {} args, and got {}.", placeholdersCount, sizeof...(args)));
    std::string logMsg(format);
    size_t argIndex = 0;
    ((logMsg.replace(logMsg.find("{}"), 2, toString(args)), ++argIndex), ...);
    return logMsg;
}

/**
 * Not Thread Safe
 */
template <typename... Args>
void printFormatedString(std::string_view format, const Args &...args) {
    std::cout << formatStr(format, args...) << std::endl;
}

/// MARK: string util

inline vector<string> splitString(string_view input, string_view delim) {
    auto delimlen = delim.size();
    vector<string> ret;
    std::remove_const_t<decltype(string::npos)> prev = 0;
    auto p = prev;
    while (true) {
        p = input.find(delim, p);
        ret.emplace_back(input.substr(prev, p - prev));
        if (p == string::npos) {
            break;
        }
        p += delimlen;
        prev = p;
    }
    return ret;
}

inline string trimString(string str, string_view trimed = R"( []")") {
    set<char> trimedChar(trimed.begin(), trimed.end());
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [&trimedChar](char ch) {
                  return trimedChar.find(ch) == trimedChar.end();
              }));
    str.erase(std::find_if(str.rbegin(), str.rend(),
                           [&trimedChar](char ch) {
                               return trimedChar.find(ch) == trimedChar.end();
                           })
                  .base(),
              str.end());
    return str;
}

inline vector<int> getMaxPrefixArray(const string &str) {
    int length = str.size();
    vector<int> phi(length, 0);
    for (int i = 1; i < length; ++i) {
        int len = phi[i - 1];
        while (len != 0 && str[i] != str[len]) {
            len = phi[len - 1];
        }
        if (str[i] == str[len]) {
            phi[i] = len + 1;
        }
    }
    return phi;
}

inline vector<int> kmpFindAllSubStr(const string &mainStr, const string &patternStr) {
    string str = patternStr + "@" + mainStr;
    vector<int> ret;
    int len = patternStr.size();
    auto prefix = getMaxPrefixArray(str);
    for (int i = 0; i < prefix.size(); i++) {
        if (prefix[i] == len) {
            ret.push_back(i - len - len);
        }
    }
    return ret;
}

inline void parseOperations(string operations, const string &datas,
                            const std::function<void(string, string)> &handler) {
    auto ops = splitString(trimString(std::move(operations), "[] "), ",");
    auto das = splitString(datas.substr(2, datas.length() - 4), "],[");
    ASSERT_MSG(ops.size() == das.size(), "The nums of Operation and Data is unequal");
    for (int i = 0; i < ops.size(); i++) {
        handler(trimString(ops[i], R"(")"), das[i]);
    }
}

inline void checkOperationsRet(string ops, const string &datas, string expected, string actual) {
    auto vop = splitString(trimString(std::move(ops)), ",");
    auto vdata = splitString(datas, "],[");
    auto vexp = splitString(trimString(std::move(expected)), ",");
    auto vactual = splitString(trimString(std::move(actual)), ",");
    ASSERT(vexp.size() == vactual.size());
    for (int i = 0; i < vexp.size(); i++) {
        if (vexp[i] != vactual[i]) {
            LOGV("op[{}]: {} {}, Exp {}, Act {}", i, vop[i], vdata[i], vexp[i], vactual[i]);
        }
    }
}

auto cout_origin_buf = std::cout.rdbuf();

inline void redirectCoutToString(stringstream &ss) {
    cout_origin_buf = std::cout.rdbuf();
    std::cout.rdbuf(ss.rdbuf());
}

inline void resetCout() {
    std::cout.rdbuf(cout_origin_buf);
}

/// MARK: random util

inline bool coinOnce() {
    auto engine = mt19937{random_device{}()};
    auto distro = uniform_real_distribution<double>(0, 1);
    return distro(engine) > 0.5;
}

inline int getRandomInt(int start = 0, int end = 100) {
    auto seed = random_device{}();
    auto engine = mt19937{seed};
    auto distro = uniform_int_distribution<int>(start, end);
    return distro(engine);
    // auto distro =
    //     normal_distribution<double>((start + end) / 2, (end - start) /
    //     6);
    // return (int)(distro(engine));
}

inline double getRandomDouble(double start = 0, double end = 1) {
    auto engine = mt19937{random_device{}()};
    auto distro = normal_distribution<double>(start, end);
    return distro(engine);
}

inline vector<int> getIntVector(string str) {
    auto tmp = splitString(trimString(str), ",");
    vector<int> ret;
    std::transform(tmp.begin(), tmp.end(), std::back_inserter(ret), [](const string &x) {
        return std::stoi(x);
    });
    return ret;
}

inline vector<int> getRandomIntVector(int len = 10, int start = 0, int end = 100, bool uniqueElem = false) {
    vector<int> vc;
    unordered_set<int> elemset;
    vc.reserve(len);
    if (uniqueElem) {
        if ((end - start + 1) < len) {
            throw std::runtime_error("Can't satisfy uniqueElem cause range is even smaller than "
                                     "length");
        }
        while (len > 0) {
            int elem = getRandomInt(start, end);
            if (elemset.find(elem) == elemset.end()) {
                elemset.emplace(elem);
                vc.push_back(elem);
                len--;
            } else {
                continue;
            }
        }
    } else {
        while (len-- > 0) {
            int elem = getRandomInt(start, end);
            vc.push_back(elem);
        }
    }
    return vc;
}

inline string getRandomString(int len = 10, bool uniqueElem = false, int charset = 5) {
    string charset1 = R"(
        !#$%&'()*+,-./"
        0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`"
        abcdefghijklmnopqrstuvwxyz{|}~)";
    string charset2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    string charset3 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string charset4 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string charset5 = "abcdefghijklmnopqrstuvwxyz";
#if charset == 1
#define CHARSET charset1
#elif charset == 2
#define CHARSET charset2
#elif charset == 3
#define CHARSET charset3
#elif charset == 4
#define CHARSET charset4
#else
#define CHARSET charset5
#endif
    stringstream ss;
    if (uniqueElem) {
        if (CHARSET.size() < len) {
            throw std::runtime_error("Can't satisfy uniqueElem cause range is even smaller than "
                                     "length");
        }
        unordered_set<int> idxSet;
        while (len > 0) {
            int idx = getRandomInt(0, CHARSET.size() - 1);
            if (idxSet.find(idx) == idxSet.end()) {
                ss << CHARSET[idx];
                len--;
            } else {
                continue;
            }
        }
    } else {
        while (len-- > 0) {
            ss << CHARSET[getRandomInt(0, CHARSET.size() - 1)];
        }
    }
    return ss.str();
}

/// MARK: boost utils

#ifdef HAS_BOOST_ENV
template <typename T>
string getTypeName(bool isRawName = false) {
    if (isRawName) {
        return boost::typeindex::type_id_with_cvr<T>().raw_name();
    }
    return boost::typeindex::type_id_with_cvr<T>().pretty_name();
}
#endif

/// MARK: Graph
namespace graphConst {
const int dummyValue = 0;
const int wuxiangtuWeight = 1;
};  // namespace graphConst

template <Printable ValueType>
class Graph {
public:
    vector<ValueType> nodes;
    vector<vector<int>> edges;
    bool isYouXiang;
    int nodeNum;

public:
    Graph(bool isYouXiangTu = false) : isYouXiang(isYouXiangTu), nodeNum(0) {
    }

    Graph(vector<ValueType> v, bool isYouXiangTu = false)
        : nodes(std::move(v)),
          isYouXiang(isYouXiangTu),
          nodeNum(nodes.size()),
          edges(vector<vector<int>>(nodes.size(), vector<int>(nodes.size(), graphConst::dummyValue))) {
    }

    Graph(vector<ValueType> v, vector<vector<int>> e, bool isYouXiangTu = false)
        : nodes(std::move(v)), edges(std::move(e)), nodeNum(nodes.size()), isYouXiang(isYouXiangTu) {
        if (!isYouXiang) {
            for (int i = 0; i < nodeNum; i++) {
                for (int j = i + 1; j < nodeNum; j++) {
                    edges[i][j] = edges[j][i];
                }
            }
        }
    }

    int size() const {
        return nodeNum;
    }

    bool empty() const {
        return nodeNum == 0;
    }

    Graph<ValueType> &addNode(ValueType val) {
        nodes.emplace_back(std::move(val));
        ++nodeNum;
        edges.emplace_back(nodeNum, graphConst::dummyValue);
        for (int i = 0; i < nodeNum - 1; ++i) {
            edges[i].resize(nodeNum, graphConst::dummyValue);
        }
        return *this;
    }

    Graph<ValueType> &addEdge(int from, int to, int value = 1) {
        if (from >= nodeNum || to >= nodeNum) {
            throw std::out_of_range("node unexist");
        }
        if (from >= edges.size()) {
            throw std::out_of_range("edge auto expand false");
        }
        edges[from][to] = value;
        if (!isYouXiang) {
            edges[to][from] = value;
        }
        return *this;
    }

    string toString() const;
};

Graph<int> buildRandomGraph(int n = 10, bool isYouXiang = false, bool useDefaultWeight = true, int start = 1,
                            int end = 100) {
    vector<int> vertex = getRandomIntVector(n, start, end);
    int dummy = graphConst::dummyValue;
    vector<vector<int>> adjust(n, vector<int>(n, dummy));
    for (int i = 0; i < n * 3; i++) {
        vector<int> tup = getRandomIntVector(2, 0, n - 1);
        if (!useDefaultWeight) {
            adjust[tup[0]][tup[1]] = getRandomInt(start, end);
        } else {
            adjust[tup[0]][tup[1]] = graphConst::wuxiangtuWeight;
        }
    }
    if (!isYouXiang) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                if (adjust[i][j] != adjust[j][i]) {
                    if (coinOnce()) {
                        adjust[i][j] = adjust[j][i];
                    } else {
                        adjust[j][i] = adjust[i][j];
                    }
                }
            }
        }
    }
    return Graph<int>(vertex, adjust, isYouXiang);
}

template <Printable ValueType>
string Graph<ValueType>::toString() const {
    stringstream ss;
    ss << "[nodes]:\n\t" << ::toString(nodes) << "\n";
    ss << "[edges]:\n";
    for (int i = 0; i < nodeNum; i++) {
        string tmp = formatStr("{}", edges[i]);
        ss << "\t" << tmp << "\n";
    }
    string ret = ss.str();
    return ret;
}

template <>
string Graph<int>::toString() const {
    if (nodeNum == 0) {
        return "[EMPTY GRAPH]";
    }
    int max = 1;
    for (int i = 0; i < nodeNum; i++) {
        if (nodes[i] >= 10 && nodes[i] < 100) {
            max = 2;
        }
        if (nodes[i] >= 100) {
            max = 3;
        }
    }
    max++;

    stringstream sstmp;
    for (int i = 0; i < max * 2 + 3; i++) {
        sstmp << " ";
    }
    string prefixSpace = sstmp.str();

    stringstream ss;
    ss << prefixSpace;
    for (int i = 0; i < nodeNum; i++) {
        ss << setw(max) << nodes[i];
    }
    ss << "\n";
    ss << prefixSpace;
    for (int i = 0; i < nodeNum; i++) {
        ss << setw(max) << i;
    }
    ss << "\n";
    ss << prefixSpace;
    for (int i = 0; i < nodeNum; i++) {
        ss << setw(max) << "-";
    }
    ss << "\n";
    for (int i = 0; i < nodeNum; i++) {
        ss << "[" << setw(max) << nodes[i] << "]" << setw(max) << i << "|";
        for (int j = 0; j < nodeNum; ++j) {
            if (edges[i][j] == graphConst::dummyValue) {
                ss << setw(max) << " ";
            } else if (edges[i][j] == graphConst::wuxiangtuWeight) {
                // ss << setw(max) << "O";
                ss << setw(max) << edges[i][j];
            } else {
                ss << setw(max) << edges[i][j];
            }
        }
        ss << "\n";
    }
    string ret = ss.str();
    return ret;
}

/// MARK: UnionFindSet

template <Printable T, typename Less = std::less<T>>
class UnionFindSet {
private:
    int cnt;
    map<T, T, Less> parent;
    Less lesser;

    bool equal(const T &a, const T &b) const {
        return !lesser(a, b) && !lesser(b, a);
    }

public:
    UnionFindSet() : cnt(0), lesser(Less()) {
    }

    explicit UnionFindSet(Graph<T> g) : cnt(g.size()) {
        for (auto val : g.nodes) {
            parent.emplace(val, val);
        }
        for (int i = 0; i < g.size(); i++) {
            if (g.isYouXiang) {
                for (int j = 0; j < g.size(); j++) {
                    if (g.edges[i][j] != graphConst::dummyValue) {
                        connect(g.nodes[i], g.nodes[j]);
                    }
                }
            } else {
                for (int j = 0; j < i; j++) {
                    if (g.edges[i][j] != graphConst::dummyValue) {
                        connect(g.nodes[i], g.nodes[j]);
                    }
                }
            }
        }
    }

    int count() const {
        return cnt;
    }

    void add(T val) {
        auto [it, yes] = parent.emplace(val, val);
        if (yes) {
            ++cnt;
        }
    }

    void add(T key, T val) {
        auto [it1, yes1] = parent.emplace(key, key);
        if (yes1) {
            ++cnt;
        }
        auto [it2, yes2] = parent.emplace(val, val);
        if (yes2) {
            ++cnt;
        }
        connect(key, val);
    }

    T find(T key) {
        if (parent.find(key) == parent.end()) {
            throw std::out_of_range(formatStr("key {} Unexist.", key));
        }
        T p = parent[key];
        if (equal(p, key)) {  // equal要求const参数，传递parent[key]会导致map类型变为const map，进而无法引用
            return key;
        }
        return find(p);
    }

    void connect(T key1, T key2) {
        T r1 = find(key1);
        T r2 = find(key2);
        if (!equal(r1, r2)) {
            parent[r1] = r2;
            --cnt;
        }
    }

    string toString() const {
        if (cnt == 0) {
            return "[]"s;
        }
        vector<T> top;
        for (auto [k, v] : parent) {
            if (equal(k, v)) {
                top.emplace_back(k);
            }
        }
        stringstream ss;
        for (auto n : top) {
            ss << toStringHelper(n) << "\n";
        }
        string ret = ss.str();
        return ret;
    }

    string toStringHelper(T a) const {
        stringstream ss;
        ss << formatStr("{}", a) << "-(";
        for (auto [k, v] : parent) {
            if (equal(v, a) && !equal(k, a)) {
                ss << toStringHelper(k) << ",";
            }
        }
        string ret = ss.str();
        if (ret.back() == ',') {
            ret.pop_back();
            ret += ")";
        } else {
            ret.pop_back();
            ret.pop_back();
        }
        return ret;
    }
};

template <>
class UnionFindSet<int> {
private:
    int cnt;
    vector<int> parent;

public:
    UnionFindSet(int n) : cnt(n), parent(n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    UnionFindSet(Graph<int> g) : cnt(g.size()), parent(cnt) {
        for (int i = 0; i < cnt; i++) {
            parent[i] = i;
        }
        int len = g.size();
        for (int i = 0; i < len; i++) {
            if (g.isYouXiang) {
                for (int j = 0; j < len; j++) {
                    if (g.edges[i][j] != graphConst::dummyValue) {
                        connect(i, j);
                    }
                }
            } else {
                for (int j = 0; j < i; j++) {
                    if (g.edges[i][j] != graphConst::dummyValue) {
                        // LOGV("connect {}<->{}", i, j);
                        connect(i, j);
                    }
                }
            }
        }
    }

    int count() const {
        return cnt;
    }

    int find(int a) {
        // 标准的采用路径压缩算法，以尽量减少树的高度
        if (parent[a] != a) {
            parent[a] = find(parent[a]);
        }
        return parent[a];
    }

    UnionFindSet &connect(int a, int b) {
        int ra = find(a);
        int rb = find(b);
        if (ra != rb) {
            parent[rb] = ra;
            cnt--;
        }
        return *this;
    }

    void normalize() {
        for (int i = 0; i < parent.size(); i++) {
            if (parent[i] != i) {
                parent[i] = find(parent[i]);
            }
        }
    }

    bool isConnected(int a, int b) {
        return find(a) == find(b);
    }

    string toString() const {
        vector<int> top;
        for (int i = 0; i < parent.size(); i++) {
            if (parent[i] == i) {
                top.push_back(i);
            }
        }
        stringstream ss;
        for (auto n : top) {
            ss << toStringHelper(n) << "\n";
        }
        string ret = ss.str();
        return ret;
    }

    string toStringHelper(int a) const {
        stringstream ss;
        ss << a << "-(";
        for (int i = 0; i < parent.size(); i++) {
            if (parent[i] == a && i != a) {
                ss << toStringHelper(i) << ",";
            }
        }
        string ret = ss.str();
        if (ret.back() == ',') {
            ret.pop_back();
            ret += ")";
        } else {
            ret.pop_back();
            ret.pop_back();
        }
        return ret;
    }
};

/// MARK: Heap

template <typename ValueType, typename Compare = std::greater<>>
class Heap {
private:
    Compare comp;
    std::vector<ValueType> data;
    int elemNums;

private:
    void buildHeap();
    void adjustUp(int k);
    void adjustDown(int k);

public:
    Heap() : comp(Compare()), data(1, ValueType{}), elemNums(0) {
    }

    Heap(std::vector<ValueType> vals) : comp(Compare()), data(std::move(vals)), elemNums(data.size()) {
        data.insert(data.begin(), ValueType{});
        buildHeap();
    }

    explicit Heap(const Compare &cmp) : comp(cmp), data(1, ValueType{}), elemNums(0) {
    }

    ValueType &top();

    void pop() noexcept;
    void push(ValueType val);

    bool empty() const {
        return elemNums == 0;
    }

    int size() const {
        return elemNums;
    }

    std::string toString() const {
        return ::toString(std::vector<ValueType>(data.begin() + 1, data.begin() + elemNums));
    }

public:
    static void sort(vector<ValueType> &vc);
};

template <typename ValueType, typename Compare>
void Heap<ValueType, Compare>::buildHeap() {
    for (int i = elemNums / 2; i >= 1; --i) {
        adjustDown(i);
    }
}

template <typename ValueType, typename Compare>
void Heap<ValueType, Compare>::adjustDown(int k) {
    while (2 * k <= elemNums) {
        int j = 2 * k;
        if (j < elemNums && comp(data[j], data[j + 1])) {
            j++;
        }
        if (!comp(data[k], data[j])) {
            break;
        }
        std::swap(data[k], data[j]);
        k = j;
    }
}

template <typename ValueType, typename Compare>
void Heap<ValueType, Compare>::adjustUp(int k) {
    while (k > 1 && comp(data[k], data[k / 2])) {
        std::swap(data[k], data[k / 2]);
        k /= 2;
    }
}

template <typename ValueType, typename Compare>
ValueType &Heap<ValueType, Compare>::top() {
    if (elemNums >= 1) {
        return data[1];
    }
    throw std::out_of_range("Heap Empty");
}

template <typename ValueType, typename Compare>
void Heap<ValueType, Compare>::pop() noexcept {
    if (elemNums > 0) {
        std::swap(data[1], data[elemNums]);
        --elemNums;
        adjustDown(1);
    }
}

template <typename ValueType, typename Compare>
void Heap<ValueType, Compare>::push(ValueType val) {
    ++elemNums;
    data.resize(elemNums + 1);
    data[elemNums] = std::move(val);
    adjustUp(elemNums);
}

template <typename ValueType, typename Compare>
void Heap<ValueType, Compare>::sort(vector<ValueType> &vc) {
    vector<ValueType> ret;
    auto heap = Heap<ValueType, Compare>(vc);
    while (!heap.empty()) {
        ret.push_back(heap.top());
        heap.pop();
    }
    vc.assign(ret.begin(), ret.end());
}

/// MARK: SkipList

template <typename K, typename V>
struct SkipListNode {
    K key;
    V val;

    int level;
    std::vector<SkipListNode *> nextNodes;

    SkipListNode(int level, K k = K{}, V v = V{})
        : key(k), val(v), level(level), nextNodes(vector<SkipListNode *>(level, nullptr)) {
    }
};

template <typename K, typename V>
class SkipList {
public:
    SkipList() : node_num(0), head(new SkipListNode<K, V>(max_level)) {
    }

    SkipListNode<K, V> *find(K key) const;
    V &operator[](K key);
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
    SkipListNode<K, V> *head;
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
SkipListNode<K, V> *SkipList<K, V>::find(K key) const {
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
V &SkipList<K, V>::operator[](K key) {
    auto r = this->find(key);
    if (r == nullptr) {
        throw std::runtime_error("Unexists Key");
    }
    return r->val;
}

template <typename K, typename V>
bool SkipList<K, V>::insert(K key, V val) {
    auto curr = head;
    std::vector<SkipListNode<K, V> *> update(max_level, nullptr);
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
    std::vector<SkipListNode<K, V> *> update(max_level, nullptr);
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

#endif  // SHUAIKAI_LC_H
