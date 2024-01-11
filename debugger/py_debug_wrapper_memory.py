

def run_debugger():
    import sys

    sys.path.insert(1, "/home/bergschaf/PycharmProjects/Pyduino2/riscv_interpreter/")
    import main
    filname = "/home/bergschaf/PycharmProjects/Pyduino2/riscv_emulator/test"
    kernel = main.init(filname)
    kernel.log_level = 3
    kernel.disable_breakpoints = True
    file = open("py_memory","w")
    file.write("\n".join([str(bin(i))[2:] for i in kernel.memory.memory]))

if __name__ == '__main__':
    run_debugger()
