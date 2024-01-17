from interpreter import Registers, Instructions, int_to_bin
from memory import Memory
from elf_loader import ELF_File
import termcolor
from kernel import Kernel

LOG_LEVEL = 2
BREAKPOINTS = []
DISABLE_BREAKPOINTS = True
STOP_EVERY_1000 = False


def init(file):
    elf = ELF_File(file).load()

    to_load = elf.get_program_data()

    # load the Segments specified in the program headers
    memory_size = 100000000  # last segment's virtual address + size
    memory = Memory(memory_size)
    memory.start_adress = 0 # TODO

    with open(file, "rb") as f:
        f = f.read()
        for segment in to_load:
            print("vaddr: ", hex(segment[2]), "  size: ", hex(segment[1]), " offset: ", hex(segment[0]))
            memory.store_bytes(segment[2], f[segment[0]:segment[0] + segment[1]])

    # initialize registers
    registers = Registers()
    registers.pc = elf.entry_pos
    kernel = Kernel(memory, registers, elf, log_level=LOG_LEVEL)
    kernel.disable_breakpoints = DISABLE_BREAKPOINTS
    kernel.log("Memory size: ", memory_size, priority=2)

    # set the stack pointer
    kernel.registers[2] = memory_size - 1000

    return kernel


def run_next(kernel):
    inst = kernel.memory.load_word(kernel.registers.pc)
    kernel.log(f"Instruction: {hex(inst)} | {inst:032b}")
    kernel.log(f"PC: {kernel.registers.pc} | " + termcolor.colored(f'0x{int_to_bin(kernel.registers.pc):08X}', 'white',
                                                                   force_color=True))
    prev_pc = kernel.registers.pc
    instruction = Instructions.decode(inst, kernel)
    if kernel.registers.pc not in Instructions.CACHE:
        Instructions.CACHE[kernel.registers.pc] = instruction
    kernel.log(instruction)
    res = instruction.do(kernel)
    if res is not None:
        return res
    kernel.breakpoint()

    if prev_pc in BREAKPOINTS:
        kernel.breakpoint()


    if kernel.registers.pc == prev_pc:
        kernel.registers.pc += 4


def run(kernel):
    num_instructions = 0
    while True:
        res = run_next(kernel)
        if res is not None:
            return res
        num_instructions += 1
        kernel.log("Registers:\n", kernel.registers)
        kernel.log(f"Instruction count: {num_instructions}\n\n")
        if STOP_EVERY_1000 and num_instructions % 1000 == 0:
            input()


def run_test_file(file):
    # end with ebreak instruction
    # ONLY FOR TESTING
    kernel = init(file)
    kernel.testing = True
    kernel.log_level = 3
    kernel.disable_breakpoints = True
    run(kernel)
    return kernel.registers, kernel.memory


if __name__ == '__main__':
    file = "../arduino_riscv_emulator/test_print"
    kernel = init(file)
    # 0x156cc
    #exit()

    run(kernel)

    # 11f88 -> evlt stdout ecall
    # 12014 -> evlt stio ecall
