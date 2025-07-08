#pragma once

#include <fstream>
#include <string>

#include "session.hh"

namespace sshfs2 {

class SSHFuseOp {
 public:
  explicit SSHFuseOp(std::unique_ptr<Session> session,
                     const std::string& filename = "log.txt");

  void BindOperations(struct fuse_operations& ops) {
    ops.readdir = &SSHFuseOp::ReadDir;
    ops.getattr = &SSHFuseOp::GetAttr;
    ops.open = &SSHFuseOp::Open;
    ops.opendir = &SSHFuseOp::OpenDir;
    ops.releasedir = &SSHFuseOp::ReleaseDir;
  }

 private:
  static SSHFuseOp* Instance() {
    return static_cast<SSHFuseOp*>(fuse_get_context()->private_data);
  }

  static int Open(const char* path, struct fuse_file_info* fi) {
    auto inst = Instance();

    return inst->OpenImpl(path, fi);
  }

  /** Read directory
   *
   * This supersedes the old getdir() interface.  New applications
   * should use this.
   *
   * The filesystem may choose between two modes of operation:
   *
   * 1) The readdir implementation ignores the offset parameter, and
   * passes zero to the filler function's offset.  The filler
   * function will not return '1' (unless an error happens), so the
   * whole directory is read in a single readdir operation.  This
   * works just like the old getdir() method.
   *
   * 2) The readdir implementation keeps track of the offsets of the
   * directory entries.  It uses the offset parameter and always
   * passes non-zero offset to the filler function.  When the buffer
   * is full (or an error happens) the filler function will return
   * '1'.
   *
   * Introduced in version 2.3
   */

  /// Read directory contents and fill them into the buffer for the kernel.
  ///
  /// \param path The path of the directory to be read (e.g., "/home/user").
  /// \param buf The buffer used by FUSE to store directory entries.
  /// \param filler Callback function used to add directory entries to the
  ///               buffer.
  /// \param offset Offset into the directory stream (ignored in most cases).
  /// \param fi File information structure associated with the directory
  ///           (optional).
  /// \param flags Flags indicating read options.
  /// \return 0 on success, or a negative errno value on error.
  static int ReadDir(const char* path, void* buf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info* fi,
                     enum fuse_readdir_flags flag) {
    std::ofstream ofs("a.txt");
    ofs << "ReadDir is called\n";

    auto inst = Instance();

    ofs << "inst: " << inst << std::endl;
    ofs.flush();
    ofs.close();

    return inst->ReadDirImpl(path, buf, filler, offset, fi, flag);
  }

  static int GetAttr(const char* path, struct stat* st,
                     struct fuse_file_info* fi) {
    auto inst = Instance();

    return inst->GetAttrImpl(path, st, fi);
  }

  int OpenImpl(const char* path, struct fuse_file_info* fi);

  int ReadDirImpl(const char* path, void* buf, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info* fi,
                  enum fuse_readdir_flags flag);

  int GetAttrImpl(const char* path, struct stat* st, struct fuse_file_info* fi);

  static int OpenDir(const char* path, struct fuse_file_info* fi) {
    std::ofstream ofs("a.txt", std::ios::app);
    ofs << "OpenDir called for: " << path << "\n";
    ofs.close();

    auto inst = Instance();
    return inst->OpenDirImpl(path, fi);
  }

  static int ReleaseDir(const char* path, struct fuse_file_info* fi) {
    std::ofstream ofs("a.txt", std::ios::app);
    ofs << "ReleaseDir called for: " << path << "\n";
    ofs.close();

    auto inst = Instance();
    return inst->ReleaseDirImpl(path, fi);
  }

  int OpenDirImpl(const char* path, struct fuse_file_info* fi) {
    // 实现目录打开逻辑
    // 可以在此处初始化目录相关的资源
    // 如果需要保存状态，可以存储在 fi->fh 中
    fi->fh = 0;  // 示例：不使用文件句柄
    return 0;    // 总是返回成功
  }

  int ReleaseDirImpl(const char* path, struct fuse_file_info* fi) {
    // 实现目录释放逻辑
    // 清理 OpenDirImpl 中分配的任何资源
    return 0;  // 总是返回成功
  }

  std::unique_ptr<Session> session_;
  std::ofstream ofs_;
};

}  // namespace sshfs2