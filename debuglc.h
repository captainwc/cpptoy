#define DEBUG 1
#if DEBUG
#include <iostream>
#include <numeric>
#include <string>
#include <type_traits>
template <typename T> struct ListType { template <typename Ptr> static auto test(Ptr ptr) -> decltype(ptr->val, ptr->next, std::true_type{}); template <typename Ptr> static std::false_type test(...); static constexpr bool value = decltype(test<T>(nullptr))::value; }; template <typename T, typename = void> struct Container : std::false_type {}; template <typename T> struct Container<T, std::void_t<typename T::value_type>> : std::true_type {}; const std::string sep = ","; template <typename T> std::string skToString(T c) { if constexpr (ListType<T>::value) { std::string ret = "["; auto p = c; while (p != nullptr && p->next != nullptr) { ret = ret + skToString(p->val) + sep; p = p->next; } if (p != nullptr) { ret += skToString(p->val); } ret += "]"; return ret; } else if constexpr (Container<T>::value && !std::is_convertible_v<T, std::string>) { if (c.empty()) { return "[]"; } return "[" + std::accumulate(std::next(c.begin()), c.end(), skToString(*(c.begin())), [](std::string a, auto b) { return a + sep + skToString(b); }) + "]"; } else if constexpr (std::is_arithmetic_v<T>) { return std::to_string(c); } else if constexpr (std::is_convertible_v<T, std::string>) { return c; } else { return "{" + skToString(c.first) + sep + skToString(c.second) + "}"; } } template <typename... Args> std::string skFmt(std::string_view format, Args... args) { std::string fmtStr(format); return ((fmtStr.replace(fmtStr.find("{}"), 2, skToString(args))), ...); } template <typename... PairTypes> void dumpWithName(PairTypes... args) { ((std::cout << "【" << skToString(std::get<0>(args)) << "】:" << skToString(std::get<1>(args)) << " "), ...); }
#define TO_PAIR(x) std::make_pair(#x, x)
#define DUMP1(x) dumpWithName(TO_PAIR(x))
#define DUMP2(x, ...) dumpWithName(TO_PAIR(x)), DUMP1(__VA_ARGS__)
#define DUMP3(x, ...) dumpWithName(TO_PAIR(x)), DUMP2(__VA_ARGS__)
#define DUMP4(x, ...) dumpWithName(TO_PAIR(x)), DUMP3(__VA_ARGS__)
#define DUMP5(x, ...) dumpWithName(TO_PAIR(x)), DUMP4(__VA_ARGS__)
#define DUMP6(x, ...) dumpWithName(TO_PAIR(x)), DUMP5(__VA_ARGS__)
#define DUMP7(x, ...) dumpWithName(TO_PAIR(x)), DUMP6(__VA_ARGS__)
#define DUMP8(x, ...) dumpWithName(TO_PAIR(x)), DUMP7(__VA_ARGS__)
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define OUTV(...) std::cout << skFmt(__VA_ARGS__) << std::endl;
#define DUMP(...) do{GET_MACRO(__VA_ARGS__, DUMP8, DUMP7, DUMP6, DUMP5, DUMP4, DUMP3,DUMP2, DUMP1)(__VA_ARGS__);std::cout << "\n";}while(0)
#else
#define OUTV(...)
#define DUMP(...)
#endif