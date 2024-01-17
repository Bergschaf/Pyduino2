//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "cpu.h"


int main() {
    Cpu *cpu = malloc(sizeof(Cpu));
    load_elf_executable("../arduino_riscv_emulator/test_print", cpu);
    run(cpu);
    return 111;
}
