from interpreter import int_from_bin, int_to_bin
from termcolor import colored


class Filesystem:
    SPECIAL_FILES = {
        "/proc/sys/kernel/osrelease": "none",
    }

    def __init__(self):
        self.files = self.SPECIAL_FILES
        self.open_files = {}

    def next_fd(self):
        if len(self.open_files) == 0:
            return 1
        return max(self.open_files.keys()) + 1  # TODO besser machen irgendwann

    def openat(self, dirfd, pathname, flags):
        if pathname not in self.files:
            # Handle Creation
            raise NotImplementedError("File creation not implemented")
        else:
            fd = self.next_fd()
            self.open_files[fd] = pathname
            return fd

    def close(self, fd):
        if fd not in self.open_files:
            raise Exception("File not open")
        del self.open_files[fd]

    def read(self, fd, count):
        # TODO implement file offsets
        if fd not in self.open_files:
            raise Exception("File not open")
        pathname = self.open_files[fd]
        if pathname not in self.files:
            raise Exception("File not found")
        if count > len(self.files[pathname]):
            print("Warning: Reading more bytes than available")
            count = len(self.files[pathname])
        return self.files[pathname][:count]


class Kernel:
    @staticmethod
    def sys_writev(kernel, a0, a1, a2, *_):
        # treat as print
        # sys_writev
        address = int_from_bin(a1)
        count = int_from_bin(a2)
        res = 0
        for i in range(count):
            iov_base = int_from_bin(kernel.memory.load_word(address + i * 16), 32)
            iov_len = kernel.memory.load_word(address + i * 16 + 8)
            byt = kernel.memory.load_bytes(iov_base, iov_len)
            # print(iov_base,iov_len)
            kernel.log(colored(bytes(byt).decode("utf-8"), "green", force_color=True), end="", priority=10)
            res += len(bytes(byt).decode("utf-8"))
        return res

    @staticmethod
    def sys_exit(kernel, a0, *_):
        kernel.log("Program exited with code ", a0, priority=3)
        exit(a0)

    @staticmethod
    def sys_brk(kernel, a0, *_):
        return -1

    @staticmethod
    def sys_getuid(kernel, *_):
        return 0

    @staticmethod
    def sys_uname(kernel, a0, *_):
        # sys_uname
        size = 65
        fillchar = "0"
        kernel.memory.puts(int_from_bin(a0), "Linux".ljust(size - 1, fillchar) + "\x00")
        kernel.memory.puts(int_from_bin(a0) + size * 1, "".ljust(size - 1, fillchar) + "\x00")
        kernel.memory.puts(int_from_bin(a0) + size * 2, "4.15.0".ljust(size - 1, fillchar) + "\x00")
        kernel.memory.puts(int_from_bin(a0) + size * 3, "".ljust(size - 1, fillchar) + "\x00")
        kernel.memory.puts(int_from_bin(a0) + size * 4, "riscv64".ljust(size - 1, fillchar) + "\x00")
        kernel.memory.puts(int_from_bin(a0) + size * 5, "".ljust(size - 1, fillchar) + "\x00")
        return -1  # TODO Change

    @staticmethod
    def sys_openat(kernel, a0, a1, a2, a3, *_):
        kernel.log("Open file at: ", kernel.memory.loads(int_from_bin(a1)), priority=2)
        return kernel.filesystem.openat(int_from_bin(a0), kernel.memory.loads(int_from_bin(a1)), int_from_bin(a2))

    @staticmethod
    def sys_read(kernel, fd, buf, count, *_):
        # sys_read
        res = kernel.filesystem.read(int_from_bin(fd), int_from_bin(count))
        kernel.memory.puts(int_from_bin(buf), res)
        return len(res)

    @staticmethod
    def sys_close(kernel, fd, *_):
        kernel.filesystem.close(int_from_bin(fd))
        return 0

    @staticmethod
    def set_tid_address(kernel, a0, *_):
        return 0

    @staticmethod
    def sys_mmap(kernel, addr, length, prot, flags, fd, offset, *_):
        print("mmap:", addr, length, prot, flags, fd, offset)
        return 0

    SYSCALL_TABLE = {
        56: sys_openat,
        57: sys_close,
        63: sys_read,
        66: sys_writev,
        93: sys_exit,
        96: set_tid_address,
        160: sys_uname,
        174: sys_getuid,
        175: sys_getuid,
        176: sys_getuid,
        177: sys_getuid,
        222: sys_mmap,
        214: sys_brk,
    }

    def __init__(self, memory, registers, elf, log_level=0):
        self.memory = memory
        self.registers = registers
        self.elf = elf
        self.log_level = log_level  # 0: not important, 1: important, 2: very important, 3: critical, 10: stdout
        self.filesystem = Filesystem()

    def log(self, *messages, priority=0, end="\n"):
        # -> higher priority = more important
        if self.log_level <= priority:
            if priority == 3:
                print(colored("".join(str(m) for m in messages), "red", attrs=["bold"], force_color=True), end=end)
            elif priority == 2:
                print(colored("".join(str(m) for m in messages), "yellow", force_color=True), end=end)
            elif priority == 1:
                print(colored("".join(str(m) for m in messages), "white", force_color=True), end=end)
            elif priority == 10:
                print(colored("".join(str(m) for m in messages), "green", force_color=True), end=end)
            else:
                print("".join(str(m) for m in messages))

    def exception(self, cause):
        self.lov_level = 0
        self.log("Exception: ", cause, priority=3)
        self.log("Program Counter: ", hex(int_to_bin(self.registers.pc)), priority=2)
        self.log("Registers:\n", self.registers)
        exit(1)

    def do_syscalL(self):
        a0 = self.registers[10]
        a1 = self.registers[11]
        a2 = self.registers[12]
        a3 = self.registers[13]
        a4 = self.registers[14]
        a5 = self.registers[15]
        n = self.registers[17]
        print("syscall: ", n)
        if n in self.SYSCALL_TABLE:
            ret = self.SYSCALL_TABLE[n](self, a0, a1, a2, a3, a4, a5, n)
        else:
            self.log("Unknown syscall ", n, priority=2)
            ret = -1
        self.registers[10] = ret
