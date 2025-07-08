#pragma once

#include <fstream>
#include <string>

#include "session.hh"

namespace sshfs2 {

class SSHFuseOp {
 public:
  explicit SSHFuseOp(std::unique_ptr<Session> session,
                     const std::string& filename = "log.txt");

  void BindOperations(struct fuse_operations& ops) { ops.readdir = ReadDir; }

 private:
  static SSHFuseOp* Instance() {
    return static_cast<SSHFuseOp*>(fuse_get_context()->private_data);
  }

  static void Open(const char* path, int flags) {
    auto inst = Instance();
    inst->DoOpen(path, flags);
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
                     off_t offset, struct fuse_file_info* fi) {
    auto inst = Instance();
    return inst->ReadDirImpl(path, buf, filler, offset, fi);
  }

  void DoOpen(const char* path, int flags) {}

  int ReadDirImpl(const char* path, void* buf, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info* fi);

  std::unique_ptr<Session> session_;
  std::ofstream ofs_;
};

}  // namespace sshfs2