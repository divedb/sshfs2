#include "session.hh"

#include "sftp.hh"

namespace sshfs2 {

Session::Session(const SSHOption& option) : option_(option) {
  if (!VerifySSHOption()) {
    return;
  }

  session_ = ssh_new();

  if (nullptr == session_) {
    LOG(WARNING) << "Failed to create SHH session.";
    return;
  }

  state_ = SessionState::kInit;
  SetSSHOption(SSH_OPTIONS_HOST, option_.host.c_str());
  SetSSHOption(SSH_OPTIONS_USER, option_.user.c_str());
}

Session::~Session() {
  if (session_) {
    ssh_free(session_);
  }
}

bool Session::Connect() {
  CHECK(state_ == SessionState::kInit);

  if (ssh_connect(session_) != SSH_OK) {
    LOG(WARNING) << "Failed to connect: " << option_.host;

    return false;
  }

  LOG(INFO) << "Succeed to connect: " << option_.host;
  state_ = SessionState::kConnected;

  return true;
}

bool Session::Authenticate(const char* passwd) {
  if (SSH_AUTH_SUCCESS !=
      ssh_userauth_password(session_, option_.user.c_str(), passwd)) {
    LOG(WARNING) << "Failed to authticate: " << GetLastError();

    return false;
  }

  LOG(INFO) << "Succeed to authenticate.";
  state_ = SessionState::kAuthenticated;

  return true;
}

SharedPtr<SFTP> Session::GetSFTP() {
  CHECK(state_ == SessionState::kAuthenticated);

  if (!sftp_) {
    auto tmp = std::make_unique<SFTP>(*this);
    sftp_ = tmp.release();
  }

  return sftp_;
}

}  // namespace sshfs2