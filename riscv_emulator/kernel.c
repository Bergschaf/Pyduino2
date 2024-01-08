//
// Created by bergschaf on 1/8/24.
//

#include "kernel.h"

int64_t do_syscall(Cpu *cpu) {
    int64_t syscall_num = cpu->regs[17];
    printf("syscall: %ld\n", syscall_num);
    return -1;
}