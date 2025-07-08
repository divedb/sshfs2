#pragma once

#include <libssh/sftp.h>

#include <vector>

#include "macro.hh"

namespace sshfs2 {

class Session;

class SFTP {
  DISABLE_COPY_AND_ASSIGN(SFTP);

 public:
  explicit SFTP(Session& session);

  ~SFTP();

  bool IsValid() const { return sftp_ != nullptr; }

  int ReadDir(const char* path, std::vector<sftp_attributes>& dentry_attrs);

 private:
  Session& owner_;
  sftp_session sftp_ = nullptr;
};

}  // namespace sshfs2