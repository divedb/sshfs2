#include <iostream>

#include "session.hh"
#include "ssh_fuse.hh"

using namespace sshfs2;

int main(int argc, char** argv) {
  SSHOption opt = {.host = "172.16.83.102", .user = "gc"};
  auto session = std::make_unique<Session>(opt);

  if (!session->IsValid()) {
    std::cout << "Session is invalid: " << session->GetLastError() << std::endl;

    return 1;
  }

  if (!session->Connect()) {
    std::cout << "Failed to connect.\n";

    return 1;
  }

  std::string passwd;
  std::cout << session->GetOption().user << '@' << session->GetOption().host
            << "'s password: ";
  std::getline(std::cin, passwd);

  if (!session->Authenticate(passwd.c_str())) {
    std::cout << "Failed to authenticate: " << session->GetLastError()
              << std::endl;

    return 1;
  }

  SSHFuseOp ssh_fuse(std::move(session));
  struct fuse_operations fuse_op;
  ssh_fuse.BindOperations(fuse_op);

  return fuse_main(argc, argv, &fuse_op, nullptr);
}