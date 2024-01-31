//
// Created by bergschaf on 1/9/24.
//
#include "cpu.h"
#include <stdio.h>
#include <unistd.h>


int main(){
    Cpu *cpu = malloc(sizeof(Cpu));
    load_elf_executable("../bin/python", cpu);
    while (1) {
        int old_stdout = dup(fileno(stdout));
        freopen ("/dev/null", "w", stdout);
        run_next(cpu);
        fflush(stdout);
        fclose(stdout);
        stdout = fdopen(old_stdout, "w");
        print_debug(cpu);
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