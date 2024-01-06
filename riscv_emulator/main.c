//
// Created by bergschaf on 1/5/24.
//

#include <stdio.h>
#include "cpu.h"


int main() {
    Cpu cpu;
    load_elf_executable("test", &cpu);
    return 43;
}
