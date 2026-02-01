#pragma once
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

template <typename T, typename Compare = std::less<T>> class BinHeap {
  private:
    std::vector<T> heap_;
    std::vector<int> indices_;
    Compare cmp_;

    void swap(int i, int j) {
        std::swap(heap_[i], heap_[j]);
        indices_[heap_[i].node] = i;
        indices_[heap_[j].node] = j;
    }

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (!cmp_(heap_[parent], heap_[idx]))
                break;
            swap(idx, parent);
            idx = parent;
        }
    }

    void siftDown(int idx) {
        int n = heap_.size();
        while (true) {
            int best = idx;
            int l = 2 * idx + 1;
            int r = 2 * idx + 2;
            if (l < n && cmp_(heap_[best], heap_[l]))
                best = l;
            if (r < n && cmp_(heap_[best], heap_[r]))
                best = r;
            if (best == idx)
                break;
            swap(idx, best);
            idx = best;
        }
    }

  public:
    BinHeap(int size) : indices_(size, -1) {}

    void push(const T& item) {
        int idx = heap_.size();
        heap_.push_back(item);
        indices_[item.node] = idx;
        siftUp(idx);
    }

    T pop() {
        if (heap_.empty())
            throw std::runtime_error("Heap is empty");
        T top = heap_.front();
        indices_[top.node] = -1;
        if (heap_.size() > 1) {
            heap_[0] = heap_.back();
            indices_[heap_[0].node] = 0;
        }
        heap_.pop_back();
        if (!heap_.empty())
            siftDown(0);
        return top;
    }

    void update(int node, const T& newItem) {
        int idx = indices_[node];
        if (idx == -1)
            throw std::runtime_error("Invalid index for update");
        T oldItem = heap_[idx];
        heap_[idx] = newItem;
        if (cmp_(oldItem, newItem))
            siftUp(idx);
        else
            siftDown(idx);
    }
    T top() const {
        if (heap_.empty())
            throw std::runtime_error("Heap is empty");
        return heap_.front();
    }

    T get(int node) const {
        int idx = indices_[node];
        if (idx == -1)
            throw std::runtime_error("Value not in heap");
        return heap_[idx];
    }
    bool empty() const { return heap_.empty(); }
};
