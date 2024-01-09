

def run_debugger():
    import sys

    sys.path.insert(1, "/home/bergschaf/PycharmProjects/Pyduino2/riscv_interpreter/")
    import main
    filname = "/home/bergschaf/PycharmProjects/Pyduino2/riscv_emulator/test"
    kernel = main.init(filname)
    kernel.log_level = 3
    kernel.disable_breakpoints = True

    while True:
        main.run_next(kernel)
        res = {}
        res["PC"] = kernel.registers.pc
        for i in range(32):
            res[i] = kernel.registers[i]
        yield res
