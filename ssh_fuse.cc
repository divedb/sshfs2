#include "ssh_fuse.hh"

#include <iostream>

#include "sftp.hh"

namespace sshfs2 {

SSHFuseOp::SSHFuseOp(std::unique_ptr<Session> session,
                     const std::string& filename)
    : session_(std::move(session)) {
  ofs_ = std::ofstream{filename, std::ios_base::out};

  if (!ofs_.is_open()) {
    throw std::exception();
  }
}

int SSHFuseOp::ReadDirImpl(const char* path, void* buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info* fi) {
  UNUSED(offset);
  UNUSED(fi);

  ofs_ << "ReadDirImpl called for path: " << path;

  auto sftp = session_->GetSFTP();

  std::vector<sftp_attributes> dentry_attrs;
  auto ret = sftp->ReadDir(path, dentry_attrs);

  if (ret) {
    return ret;
  }

  if (filler(buf, ".", NULL, 0)) return 0;
  if (filler(buf, "..", NULL, 0)) return 0;

  for (auto& attr : dentry_attrs) {
    std::cout << "attr name: " << attr->name << std::endl;
    filler(buf, attr->name, NULL, 0);
  }

  for (auto& attr : dentry_attrs) {
    sftp_attributes_free(attr);
  }

  return 0;
}

}  // namespace sshfs2