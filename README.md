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


FAQ
1. 强制卸载失败
➜  ~ sudo umount -f /Users/zlh/Desktop/alifuse
Password:
umount: pthread_cond_timeout failed; continuing with unmount: Operation timed out
umount: unmount(/Users/zlh/Desktop/alifuse): Resource busy

强制卸载（umount -f）仍然失败，并导致终端卡住，通常是由于以下原因：

1. 仍有进程占用文件系统
即使强制卸载，如果 有进程仍在访问挂载点，系统会拒绝卸载。
解决方法：

bash
# 查看哪些进程正在使用挂载点
lsof +D /Users/zlh/Desktop/alifuse

# 强制终止相关进程
kill -9 <PID>
如果 lsof 也卡住，说明某个进程已死锁，需重启系统。

2. FUSE 文件系统自身崩溃
如果 FUSE 守护进程（如 sshfs）异常退出，可能导致内核状态不一致，使卸载操作无限挂起。
解决方法：

bash
# 找到并杀死残留的 FUSE 进程
pgrep -lf sshfs  # 或 macFUSE
kill -9 <PID>

# 再次尝试卸载
sudo umount -l /Users/zlh/Desktop/alifuse
3. macOS 的 macFUSE 特殊行为
在 macOS 上，macFUSE 的卸载机制可能与 Linux 不同：

diskutil 比 umount 更可靠：

bash
diskutil unmount force /Users/zlh/Desktop/alifuse
如果仍失败，需重启 macFUSE 内核扩展：

bash
sudo kextunload -b com.github.macfuse.filesystems.macfuse
sudo kextload /Library/Filesystems/macfuse.fs/Contents/Extensions/macfuse.kext
4. 内核级文件系统锁
极端情况下，内核可能因死锁无法响应卸载请求。
终极解决方案：

重启计算机（唯一能清除内核状态的方法）。

如果频繁发生，检查 FUSE 版本或改用更稳定的实现（如 sshfs 的替代品）。

5. 终端卡住的应对措施
如果 umount -f 卡住终端：

尝试用另一个终端执行上述命令。

如果所有终端无响应：

macOS/Linux：按 Ctrl+C 多次，或关闭终端窗口。

如果仍无效，只能强制重启。

预防措施
避免直接强制卸载，优先用懒卸载：

bash
sudo umount -l /Users/zlh/Desktop/alifuse
确保无程序访问挂载点，再卸载。

对于开发中的 FUSE 文件系统，增加超时机制：

bash
sudo umount -f -t 5000 /Users/zlh/Desktop/alifuse  # 5秒超时


2. 
➜  Desktop ls alifuse
ls: alifuse: Device not configured

Q3. 为什么在目录下ls不行。➜  hissh ls
➜  hissh ls
➜  hissh cd ..
➜  Desktop ls hissh
Desktop  Documents  Downloads  Music  Pictures  Public  snap  Templates  Videos
➜  Desktop ls hissh/Desktop
a.txt
➜  Desktop 

Q4. ➜  Desktop ls hissh
ls: cannot access 'hissh': Transport endpoint is not connected
