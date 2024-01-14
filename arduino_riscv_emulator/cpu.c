//
// Created by bergschaf on 1/6/24.
//

#include "cpu.h"
#include "decoder.h"
#include "elfLoader.h"
#include "serial.h"
#include "memory.h"
#include <string.h>

void load_elf_executable(char *filename, Cpu *cpu) {
    ElfFile *file = malloc(sizeof(ElfFile));
    receive_elf_file(file);
    cpu->pc = file->entry_pos;
    // initialize the memory with 0 TODO

    // initialize the registers with 0
    for (int i = 0; i < 32; i++) {
        cpu->regs[i] = 0;
    }


    free(file);
    // Set sp to the end of the memory
    cpu->regs[2] = MEM_SIZE - 1000;
}

uint32_t get_next_inst(Cpu *cpu) {
    uint8_t bytes[4];
    memory_load(cpu->pc, bytes, 4);

    uint32_t inst = 0;
    for (int i = 0; i < 4; i++) {
        inst |= (uint32_t)bytes[i] << (8 * i);
    }
    return inst;
}


void run_next(Cpu *cpu) {
    uint32_t inst = get_next_inst(cpu);
    //serial_printf("instruction: %lx\n", inst);
    InstructionCallback callback = decode(inst);

    int64_t prev_pc = cpu->pc;

    callback.func(cpu, *callback.inst);
    cpu->regs[0] = 0; // TODO may be very broken
    //serial_printf("Inst.imm: %lx\n", callback.inst->imm);
    //print_human_debug(cpu);

    if (cpu->pc == prev_pc) {
        cpu->pc += 4;
    }

    free(callback.inst);
}

void print_human_debug(Cpu *cpu) {
    serial_printf("---------\nPC: 0x%lx\n", cpu->pc);
    print_registers(cpu);
    serial_printf("end\n\n");
}


void run(Cpu *cpu) {
    // stop and ask for input after 1000 instructions
    int i = 0;
    while (cpu->pc < MEM_SIZE) {
        run_next(cpu);
        i++;
        //if (i > 100000) {
        //    break;
        //}
    }
    serial_printf("Ran out of instructions (not good)\n");
}

void print_registers(Cpu *cpu) {
    for (int i = 0; i < 32; i++) {
        if (cpu->regs[i] != 0) {
            serial_printf("%d: 0x%lx\n", i, cpu->regs[i]);
        }
    }
}

void memory_puts(Cpu *cpu, int64_t address, char *string, int64_t size) {
    serial_printf("puts address: %lx | string: %s | size: %ld\n", address, string, size);
    memory_write(address, string, size);
}

void memory_loads(Cpu *cpu, int64_t address, char *string, int64_t size) {
    serial_printf("loads len: %ld | adress: %lx\n", size, address);
    memory_load(address, string, size);
}


int64_t memory_loadw(Cpu *cpu, int64_t address) {
    uint8_t bytes[4];
    memory_load(address, bytes, 4);
    int64_t res = 0;
    for (int i = 0; i < 4; i++) {
        res |= bytes[i] << (8 * i);
    }
    return res;
}


int64_t memory_loaddw(Cpu *cpu, int64_t address) {
    uint8_t bytes[8];
    memory_load(address, bytes, 8);
    int64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= bytes[i] << (8 * i);
    }
    return res;
}

