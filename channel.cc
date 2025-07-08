#include "channel.hh"

#include <glog/logging.h>

#include "session.hh"
#include "writer.hh"

namespace sshfs2 {

Channel::Channel(Session& owner) : owner_(owner) {
  chan_ = ssh_channel_new(owner.session_);

  if (nullptr == chan_) {
    return;
  }

  int rc = ssh_channel_open_session(chan_);

  if (rc != SSH_OK) {
    LOG(WARNING) << "Failed to open channel: " << ssh_get_error(owner.session_);

    return;
  }
}

bool Channel::SendRequest(const char* command, Writer& w) {
  CHECK(IsValid());

  int rc = ssh_channel_request_exec(chan_, command);

  if (rc != SSH_OK) {
    LOG(WARNING) << "Failed to send command: "
                 << ssh_get_error(owner_.session_);

    return false;
  }

  int nbytes;
  char buffer[1024];

  // TODO(gc): we may need to check the error.
  while ((nbytes = ssh_channel_read(chan_, buffer, sizeof(buffer), 0)) > 0) {
    w.Write(buffer, nbytes);
  }

  return true;
}

}  // namespace sshfs2