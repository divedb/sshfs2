#include "ssh_fuse.hh"

#include "sftp.hh"

namespace sshfs2 {

SSHFuseOp::SSHFuseOp(std::unique_ptr<Session> session)
    : session_(std::move(session)) {}

int SSHFuseOp::ReadDirImpl(const char* path, void* buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info* fi) {
  UNUSED(offset);
  UNUSED(fi);

  auto sftp = session_->GetSFTP();

  std::vector<sftp_attributes> dentry_attrs;
  auto ret = sftp->ReadDir(path, dentry_attrs);

  if (ret) {
    return ret;
  }

  for (auto& attr : dentry_attrs) {
    filler(buf, attr->name, NULL, 0);
  }

  for (auto& attr : dentry_attrs) {
    sftp_attributes_free(attr);
  }

  return 0;
}

}  // namespace sshfs2