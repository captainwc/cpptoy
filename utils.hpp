#ifndef SHUAIKAI_UTILS_HPP
#define SHUAIKAI_UTILS_HPP

#include <algorithm>
#include <cassert>
#include <climits>
#include <concepts>
#include <functional>
#include <queue>
#include <span>
#include <typeindex>

#include "lc.h"
using namespace std;

#define SORT_TEST(scale, data, X)                    \
    TIME_N(#X, scale, {                              \
        auto copy = data;                            \
        X(span(copy));                               \
        assert(is_sorted(copy.begin(), copy.end())); \
    })

namespace sort {

namespace sort_helper {

template <typename Comp, typename ElemType>
concept Comparator = requires(Comp cmp, ElemType a, ElemType b) {
    { cmp(a, b) } -> std::convertible_to<bool>;
};

template <typename ElemType, typename Comp>
    requires Comparator<Comp, ElemType>
auto partition(std::span<ElemType> sp, int pivot_pos, Comp cmp) {
    ElemType pivot = sp[pivot_pos];
    int left = 0;
    int right = sp.size() - 1;

    std::swap(sp[pivot_pos], sp[right]);  // Move pivot to end
    int store_index = left;
    for (int i = left; i < right; ++i) {
        if (cmp(sp[i], pivot)) {
            std::swap(sp[i], sp[store_index]);
            store_index++;
        }
    }
    std::swap(sp[store_index], sp[right]);  // Move pivot to its final place
    return store_index;
}
};  // namespace sort_helper

template <typename ElemType, typename Comp = std::less<ElemType>>
    requires sort_helper::Comparator<Comp, ElemType>
void bubbleSort(span<ElemType> sp, Comp cmp = Comp()) {
    int len = sp.size();
    for (int i = 0; i < len - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < len - i - 1; j++) {
            if (cmp(sp[j + 1], sp[j])) {
                swap(sp[j], sp[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
    assert(is_sorted(sp.begin(), sp.end()));
}

template <typename ElemType, typename Comp = std::less<ElemType>>
    requires sort_helper::Comparator<Comp, ElemType>
void seleceSort(span<ElemType> sp, Comp cmp = Comp()) {
    int len = sp.size();
    for (int i = 0; i < len - 1; i++) {
        int min = INT_MAX;
        int imin = -1;
        for (int j = i; j < len; j++) {
            if (cmp(sp[j], min)) {
                imin = j;
                min = sp[j];
            }
        }
        swap(sp[imin], sp[i]);
    }
    assert(is_sorted(sp.begin(), sp.end()));
}

template <typename ElemType, typename Comp = std::less<ElemType>>
    requires sort_helper::Comparator<Comp, ElemType>
void quickSort(std::span<ElemType> sp, Comp cmp = Comp()) {
    int len = sp.size();
    if (len <= 1)
        return;

    int pivot_idx = len / 2;
    auto partition_pos = sort_helper::partition(sp, pivot_idx, cmp);

    quickSort(sp.subspan(0, partition_pos), cmp);
    quickSort(sp.subspan(partition_pos + 1), cmp);
}

template <typename ElemType, typename Comp = std::less<ElemType>>
    requires sort_helper::Comparator<Comp, ElemType>
void mergeSort(span<ElemType> sp, Comp cmp = Comp()) {
    int len = sp.size();
    if (len <= 1) {
        return;
    }
    int mid = len / 2;
    mergeSort(sp.subspan(0, mid), cmp);
    mergeSort(sp.subspan(mid + 1), cmp);
    merge(sp.subspan(0, mid), sp.subspan(mid + 1));
}

};  // namespace sort

namespace algorithm {
template <typename MutableIterator, typename Predicit>
MutableIterator partition(MutableIterator start, MutableIterator end, Predicit pred) {
    if (start == end) {
        return start;
    }
    --end;  // 不要最后一个元素
    while (start < end) {
        while (start < end && pred(*start)) {
            ++start;
        }
        while (start < end && !(pred(*end))) {
            --end;
        }
        if (start < end) {
            std::iter_swap(start, end);
            ++start;
            --end;
        }
    }
    return start;
}

template <typename MutableIterator>
void quickSort(MutableIterator begin, MutableIterator end) {
    if (begin >= end) {
        return;
    }
    auto pivot = *begin;
    auto pos = algorithm::partition(begin, end, [pivot](const auto& elem) {
        return elem < pivot;
    });
    auto pivot_pos = algorithm::partition(pos, end, [pivot](const auto& elem) {
        return elem <= pivot;
    });
    algorithm::quickSort(begin, pivot_pos);
    algorithm::quickSort(pivot_pos + 1, end);
}
};  // namespace algorithm

#endif  // SHUAIKAI_UTILS_HPP
