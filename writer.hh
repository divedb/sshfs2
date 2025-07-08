#pragma once

#include <cstddef>

namespace sshfs2 {

class Writer {
 public:
  virtual int Write(const char* buffer, size_t size) = 0;
};

}  // namespace sshfs2