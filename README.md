load from yaml

➜  alifuse umount /Users/zlh/Desktop/alifuse 
umount(/Users/zlh/Desktop/alifuse): Resource busy -- try 'diskutil unmount'

2. 确保终端不在挂载目录内
如果当前终端的工作目录是挂载点或其子目录，也会导致 Resource busy 错误：

bash
cd ~  # 切换到其他目录
sudo umount /Users/zlh/Desktop/alifuse

为什么终端在挂载目录内会导致 Resource busy 错误？
1. 操作系统机制
挂载点（Mount Point） 是文件系统中的一个特殊目录，代表另一个存储设备（如 FUSE 虚拟文件系统）的入口。

卸载（umount） 时，操作系统会检查该目录是否被任何进程占用（如文件访问、工作目录等）。

如果挂载点或其子目录是某个进程的 当前工作目录（Current Working Directory, cwd），系统会拒绝卸载，否则可能导致进程行为异常（如访问不存在的路径）。

2. 终端（Shell）的影响
当你在终端中执行 cd /Users/zlh/Desktop/alifuse 时，Shell 进程（如 zsh/bash）的工作目录就变成了这个挂载点。

此时运行 sudo umount，umount 进程本身也会继承这个工作目录（因为 sudo 默认保留环境变量和当前目录）。

因此，挂载点被 Shell 和 umount 自身同时占用，系统检测到冲突，返回 Resource busy。# sshfs2
