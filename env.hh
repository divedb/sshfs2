#pragma once

#include <cstdlib>

namespace sshfs2 {

class Env {
 public:
  static const char* GetUserName() {
#ifdef _WIN32
    const char* username = std::getenv("USERNAME");
#else
    const char* username = std::getenv("USER");
#endif

    return username;
  }
};

}  // namespace sshfs2