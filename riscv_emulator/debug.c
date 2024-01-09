//
// Created by bergschaf on 1/9/24.
//
#include "cpu.h"

int main(){
    Cpu cpu;
    load_elf_executable("test", &cpu);
    while (1) {
        run_next(&cpu);
        print_debug(&cpu);
        fflush(stdout);
        // wait for stdin
        char c = getchar();
        if (c == 'n') {
            continue;
        } else {
            break;
        }
    }
    return 0;
}