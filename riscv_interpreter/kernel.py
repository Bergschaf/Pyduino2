from interpreter import int_from_bin
from termcolor import colored

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
            kernel.log(colored(bytes(byt).decode("utf-8"), "green", force_color=True), end="")
            res += len(bytes(byt).decode("utf-8"))
        return res

    SYSCALL_TABLE = {
        64: sys_writev
    }

    def __init__(self, memory, registers, elf, log_level=0):
        self.memory = memory
        self.registers = registers
        self.elf = elf
        self.log_level = log_level  # 0: not important, 1: important, 2: very important, 3: critical, 10: stdout

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

    def do_syscalL(self):
        a0 = self.registers[10]
        a1 = self.registers[11]
        a2 = self.registers[12]
        a3 = self.registers[13]
        a4 = self.registers[14]
        a5 = self.registers[15]
        n = self.registers[17]
        if n in self.SYSCALL_TABLE:
            ret = self.SYSCALL_TABLE[n](self, a0, a1, a2, a3, a4, a5, n)
        else:
            self.log("Unknown syscall ", n, priority=3)
            ret = a0
        self.registers[10] = ret
