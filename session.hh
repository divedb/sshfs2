#pragma once

#include <glog/logging.h>

#include <string>

#include "channel.hh"
#include "env.hh"
#include "macro.hh"
#include "refcount.hh"

namespace sshfs2 {

class SFTP;

struct SSHOption {
  /// The hostname or ip address to connect to (const char *).
  std::string host;
  /// The username for authentication (const char *).
  std::string user;
  /// The port to connect to (unsigned int).
  unsigned int port = 22;
  const char* port_str = "22";

  /// Set the ssh directory (const char *).
  /// If the value is NULL, the directory is set to the default ssh directory.
  ///
  /// The ssh directory is used for files like known_hosts and identity (private
  /// and public key). It may include "%s" which will be replaced by the user
  /// home directory.
  const char* ssh_dir = nullptr;
};

class Session {
  friend class Channel;
  friend class SFTP;

  DISABLE_COPY_AND_ASSIGN(Session);

 public:
  enum class SessionState { kInit, kConnected, kAuthenticated, kError };

  explicit Session(const SSHOption& option);

  ~Session();

  bool Connect();

  bool Authenticate(const char* passwd);

  std::string GetLastError() const {
    CHECK(IsValid());

    return ssh_get_error(session_);
  }

  bool IsValid() const { return session_ != nullptr; }

  const SSHOption& GetOption() const { return option_; }

  SharedPtr<SFTP> GetSFTP();

 private:
  bool VerifySSHOption() {
    if (option_.host.empty()) {
      LOG(WARNING) << "Host option can't be empty.";

      return false;
    }

    if (option_.user.empty()) {
      auto uname = Env::GetUserName();

      if (nullptr == uname) {
        LOG(WARNING) << "User option is empty and can't get username from env.";

        return false;
      }

      option_.user = uname;
    }

    return true;
  }

  void SetSSHOption(enum ssh_options_e type, const void* value) {
    if (ssh_options_set(session_, type, value)) {
      LOG(WARNING) << "Failed to set ssh option: [" << type << ',' << value
                   << ']';
    }
  }

  SSHOption option_;
  ssh_session session_;
  SessionState state_;
  SharedPtr<SFTP> sftp_;
};

}  // namespace sshfs2