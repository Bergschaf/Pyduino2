//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "cpu.h"


int main() {
    Cpu *cpu = malloc(sizeof(Cpu));
    load_elf_executable("test", cpu);
    printf("PC: %lx\n", cpu->pc);
    exit(0);
    run(cpu);
    return 43;
}
