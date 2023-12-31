from interpreter import Registers, Instructions
from memory import Memory
from elf_loader import ELF_File
import termcolor
from kernel import Kernel

OUT = False
LITTLE_OUT = True

def init(file):
    elf = ELF_File(file).load()

    to_load = elf.get_program_data()

    # load the Segments specified in the program headers
    memory_size = to_load[-1][2] + to_load[-1][1]  + 20000 # last segment's virtual address + size
    if OUT or LITTLE_OUT: print(f"Memory size: {memory_size} bytes")
    memory = Memory(memory_size)
    with open(file, "rb") as f:
        f = f.read()
        for segment in to_load:
            memory.store_bytes(segment[2], f[segment[0]:segment[0] + segment[1]])

    # initialize registers
    registers = Registers()
    registers.pc = elf.entry_pos
    kernel = Kernel(memory, registers, elf)
    return kernel

def run_next(kernel):
    inst = kernel.memory.load_word(kernel.registers.pc)
    kernel.log(f"Instruction: {inst:08X} | {inst:032b}")
    kernel.log(f"PC: {kernel.registers.pc} | " + termcolor.colored(f'0x{kernel.registers.pc:08X}','white', force_color=True))
    prev_pc = kernel.registers.pc
    instruction = Instructions.match(inst)
    kernel.log(instruction)
    res = instruction.do(kernel.registers)
    if res is not None:
        return res
    if kernel.registers.pc == prev_pc:
        kernel.registers.pc += 4

def run(kernel):
    num_instructions = 0
    while True:
        res = run_next(kernel)
        if res is not None:
            return res
        num_instructions += 1
        kernel.log("Registers:\n", registers)
        kernel.log(f"Instruction count: {num_instructions}\n\n")

def run_test_file(file):
    # end with ebreak instruction
    # ONLY FOR TESTING
    kernel = init(file)
    kernel.registers.testing = True
    run(kernel)
    return kernel.registers,kernel.memory



if __name__ == '__main__':
    file = "test"
    kernel = init(file)
    run(kernel)

    #11f88 -> evlt stdout ecall
    #12014 -> evlt stio ecall


