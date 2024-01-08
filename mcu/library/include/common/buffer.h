/**
 * @file buffer.h
 * @brief
 */

#pragma once

#include <string.h>

#include <vector>

namespace common {

template <typename T>
class Buffer {
private:
  std::vector<T> buffer_;
  size_t size_;
  size_t index_ = 0;

public:
  Buffer(size_t size) : size_(size) { buffer_.resize(size); }
  void addBuffer(T data) {
    buffer_[index_] = data;
    index_++;
  }

  void addBuffer(T *data, size_t length) {
    memcpy(getBufferPointer(), data, length * sizeof(T));
    index_ += length;
  }

  size_t copyBuffer(T *buffer_pointer) {
    memcpy(buffer_pointer, getBufferPointer(), index_ * sizeof(T));
    return index_;
  }

  void clear() { index_ = 0; }

  T *getBufferPointer() { return buffer_.data(); }

  size_t getLength() { return index_; }

  size_t getBufferSize() { return buffer_.size(); }
};

}  // namespace common
