from interpreter import Registers, Instructions
from memory import Memory
from elf_loader import ELF_File
import termcolor

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
    registers.memory = memory

    return memory, registers, elf

def run_next(memory, registers):
    inst = memory.load_word(registers.pc)
    if OUT:
        print(f"Instruction: {inst:08X} | {inst:032b}")
        print(f"PC: {registers.pc} | " + termcolor.colored(f'0x{registers.pc:08X}','white', force_color=True))
    prev_pc = registers.pc
    instruction = Instructions.match(inst)
    if OUT: print(instruction)
    res = instruction.do(registers)
    if res is not None:
        return res
    if registers.pc == prev_pc:
        registers.pc += 4

def run(memory, registers, elf):
    num_instructions = 0
    while True:
        res = run_next(memory, registers)
        if res is not None:
            return res
        num_instructions += 1
        if OUT:
            print("\nRegisters:")
            print(registers)
            print(f"Instruction count: {num_instructions}")
            print()
            print()

def run_test_file(file):
    # end with ebreak instruction
    # ONLY FOR TESTING
    memory, registers, elf = init(file)
    registers.testing = True
    run(memory, registers, elf)
    return registers, memory



if __name__ == '__main__':
    file = "test"
    memory, registers, elf = init(file)
    run(memory, registers, elf)

    #11f88 -> evlt stdout ecall
    #12014 -> evlt stio ecall


