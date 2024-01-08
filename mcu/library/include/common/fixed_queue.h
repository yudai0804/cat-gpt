/**
 * @file fixed_queue.h
 * @brief 固定長のqueue
 */

#pragma once

#include <cstddef>
#include <vector>

namespace common {

/**
 * @brief 固定長のqueue
 * リングバッファが一周して、front=rearとなるとおかしくなるので注意。
 *
 */
template <typename T>
class FixedQueue {
private:
  std::vector<T> queue_;
  size_t size_;
  size_t front_ = 0;
  size_t rear_ = 0;

private:
  void increment(size_t &i) {
    i = (i + 1) % size_;
  }

public:
  FixedQueue(size_t size) : size_(size) {
    queue_.resize(size);
  }

  bool isEmpty() {
    return (front_ == rear_) ? true : false;
  }

  void enqueue(T data) {
    queue_[front_] = data;
    increment(front_);
  }

  void enqueue(T *data, size_t len) {
    for (int i = 0; i < len; i++)
      enqueue(data[i]);
  }

  T dequeue() {
    // バッファが空だった場合は適当な値(0)を返す
    if (isEmpty())
      return 0;
    T ret = queue_[rear_];
    increment(rear_);
    return ret;
  }

  size_t getSize() { return size_; }
};

}  // namespace common
