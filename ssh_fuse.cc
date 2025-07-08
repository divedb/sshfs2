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
                           off_t offset, struct fuse_file_info* fi,
                           enum fuse_readdir_flags flag) {
  UNUSED(offset);
  UNUSED(fi);
  UNUSED(flag);

  ofs_ << "ReadDirImpl called for path: " << path;
  ofs_.flush();

  auto sftp = session_->GetSFTP();

  std::vector<sftp_attributes> dentry_attrs;
  auto ret = sftp->ReadDir(path, dentry_attrs);

  if (ret) {
    return ret;
  }

  if (filler(buf, ".", NULL, 0, static_cast<fuse_fill_dir_flags>(0))) return 0;
  if (filler(buf, "..", NULL, 0, static_cast<fuse_fill_dir_flags>(0))) return 0;

  for (auto& attr : dentry_attrs) {
    std::cout << "attr name: " << attr->name << std::endl;
    filler(buf, attr->name, NULL, 0, static_cast<fuse_fill_dir_flags>(0));
  }

  for (auto& attr : dentry_attrs) {
    sftp_attributes_free(attr);
  }

  return 0;
}

int SSHFuseOp::GetAttrImpl(const char* path, struct stat* stbuf,
                           struct fuse_file_info* fi) {
  ofs_ << "path = " << path << std::endl;

  stbuf->st_mode = S_IFDIR | 0755;
  stbuf->st_nlink = 2;

  return 0;

  // (void) fi;  // 避免未使用参数警告
  //   memset(stbuf, 0, sizeof(struct stat));

  //   // 处理根目录
  //   if (strcmp(path, "/") == 0) {
  //       stbuf->st_mode = S_IFDIR | 0755;
  //       stbuf->st_nlink = 2;
  //       stbuf->st_uid = getuid();
  //       stbuf->st_gid = getgid();
  //       stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);
  //       return 0;
  //   }

  //   // 转换路径：本地路径 → 远程路径（需自定义实现）
  //   char *remote_path = convert_to_remote_path(path);
  //   if (!remote_path) return -ENOMEM;

  //   // 通过 SFTP 获取远程文件属性
  //   sftp_attributes attr = sftp_stat(sftp_session, remote_path);
  //   free(remote_path);
  //   if (!attr) return -ENOENT;

  //   // 填充属性
  //   stbuf->st_mode = attr->permissions;
  //   stbuf->st_size = attr->size;
  //   stbuf->st_uid = getuid();
  //   stbuf->st_gid = getgid();
  //   stbuf->st_atime = attr->atime;
  //   stbuf->st_mtime = attr->mtime;
  //   stbuf->st_ctime = attr->mtime;

  //   // 如果是目录，修正 nlink 和类型
  //   if (attr->type == SSH_FILEXFER_TYPE_DIRECTORY) {
  //       stbuf->st_mode = S_IFDIR | (attr->permissions & 0777);
  //       stbuf->st_nlink = 2;  // 目录至少有两个链接（"." 和 ".."）
  //   } else {
  //       stbuf->st_nlink = 1;
  //   }

  //   sftp_attributes_free(attr);
}

int SSHFuseOp::OpenImpl(const char* path, struct fuse_file_info* fi) {
  ofs_ << "open path = " << path << std::endl;

  // char* remote_path = convert_to_remote_path(path);
  // if (!remote_path) return -ENOMEM;

  // 检查文件是否存在及类型
  auto sftp = session_->GetSFTP();
  sftp_attributes attr = sftp_stat(sftp->GetSession(), path);

  if (!attr) {
    return -ENOENT;
  }

  if (attr->type != SSH_FILEXFER_TYPE_REGULAR) {
    sftp_attributes_free(attr);
    return -EISDIR;  // 不是普通文件
  }

  // 检查权限
  int flags = fi->flags & O_ACCMODE;
  if ((flags == O_WRONLY || flags == O_RDWR) &&
      (attr->permissions & 0200) == 0) {
    sftp_attributes_free(attr);

    return -EACCES;  // 无写权限
  }

  // 打开远程文件
  sftp_file file = sftp_open(sftp->GetSession(), path, fi->flags, 0);
  if (!file) {
    sftp_attributes_free(attr);

    return -EIO;
  }

  // 存储文件句柄
  fi->fh = (uint64_t)file;

  sftp_attributes_free(attr);

  return 0;
}

}  // namespace sshfs2