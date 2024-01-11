//
// Created by bergschaf on 1/9/24.
//
#include "cpu.h"
#include <stdio.h>
#include <unistd.h>


int main(){
    Cpu *cpu = malloc(sizeof(Cpu));
    load_elf_executable("test", cpu);
    int i = 0;
    while (1) {
        printf("%b\n", cpu->mem[i]);
        i++;
        if (i >= 10000000){
            break;
        }
    }
    printf("end\n");
    fflush(stdout);
    return 0;
}