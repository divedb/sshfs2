#include "sftp.hh"

#include <iostream>

#include "session.hh"

namespace sshfs2 {

SFTP::SFTP(Session& session) : owner_(session) {
  sftp_ = sftp_new(session.session_);

  if (sftp_init(sftp_) != SSH_OK) {
    LOG(WARNING) << "SFTP init failed\n";

    return;
  }
}

SFTP::~SFTP() {
  if (IsValid()) {
    sftp_free(sftp_);
  }
}

int SFTP::ReadDir(const char* path,
                  std::vector<sftp_attributes>& dentry_attrs) {
  if (!IsValid()) {
    return -EIO;
  }

  sftp_dir dir = sftp_opendir(sftp_, path);

  if (!dir) {
    LOG(WARNING) << "Cannot open remote dir: "
                 << ssh_get_error(owner_.session_);

    return -ENOENT;
  }

  sftp_attributes attr;

  while ((attr = sftp_readdir(sftp_, dir)) != NULL) {
    dentry_attrs.push_back(attr);
  }

  // TODO(gc): we need to close the directory.

  return 0;
}

}  // namespace sshfs2