from interpreter import Registers, Instructions
from memory import Memory
from elf_loader import ELF_File


def init(file):
    elf = ELF_File(file).load()

    to_load = elf.get_program_data()

    # load the Segments specified in the program headers
    memory_size = to_load[-1][2] + to_load[-1][1]  # last segment's virtual address + size
    print(f"Memory size: {memory_size} bytes")
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
    print(f"Instruction: {inst:08X}")
    print(f"PC: {registers.pc} | 0x{registers.pc:08X}")
    instruction = Instructions.match(inst)
    registers.pc += 4
    instruction.do(registers)

def run(memory, registers, elf):
    while True:
        run_next(memory, registers)
        print("\nRegisters:")
        print(registers)
        print()


if __name__ == '__main__':
    file = "test"
    memory, registers, elf = init(file)
    run(memory, registers, elf)
