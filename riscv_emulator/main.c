//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "cpu.h"


int main() {
    Cpu cpu;
    load_elf_executable("test.elf", &cpu);
    printf("pc: %lx\n", cpu.pc);
    return 43;
}
