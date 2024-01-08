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
  size_t size_;
  size_t index_ = 0;

public:
  // buffer_はpublicになっているが、直接操作するのは非推奨
  std::vector<T> buffer_;

  Buffer(size_t size) : size_(size) { buffer_.resize(size); }
  void addBuffer(T data) {
    buffer_[index_] = data;
    index_++;
  }

  void addBuffer(T *data, size_t length) {
    // lengthが0ときはmemcpyは行われない
    memcpy(getBufferPointer(), data, length * sizeof(T));
    index_ += length;
  }

  size_t copyBuffer(T *buffer_pointer) {
    // lengthが0ときはmemcpyは行われない
    memcpy(buffer_pointer, getBufferPointer(), index_ * sizeof(T));
    return index_;
  }

  void clear() {
    index_ = 0;
    memset(getBufferPointer(), 0, size_ * sizeof(T));
  }

  T *getBufferPointer() { return buffer_.data(); }

  size_t getLength() { return index_; }

  size_t getBufferSize() { return buffer_.size(); }
};

}  // namespace common
