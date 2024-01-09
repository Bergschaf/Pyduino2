//
// Created by bergschaf on 1/9/24.
//
#include "cpu.h"

int main(){
    Cpu cpu;
    load_elf_executable("test", &cpu);
    run(&cpu);
    print_debug(&cpu);
    return 0;
}