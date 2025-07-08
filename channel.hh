#pragma once

#include <libssh/libssh.h>

#include "config.hh"
#include "macro.hh"

namespace sshfs2 {

class Session;
class Writer;

class Channel {
  DISABLE_COPY_AND_ASSIGN(Channel);

 public:
  explicit Channel(Session& owner);

  ~Channel() {
    if (!IsValid()) {
      return;
    }

    ssh_channel_close(chan_);
    ssh_channel_free(chan_);
  }

  bool SendRequest(const char* command, Writer& w);

  /// \return If this channel is valid; otherwise false.
  bool IsValid() const { return chan_ != nullptr; }

 private:
  Session& owner_;
  ssh_channel chan_ = nullptr;
};

}  // namespace sshfs2