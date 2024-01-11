//
// Created by bergschaf on 1/8/24.
//

#include "kernel.h"

int64_t sys_uname(Cpu *cpu,  int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    int size = 65;
    char fillchar = '0';

    char *res = malloc(size);
    memory_puts(cpu, arg0, ljust("Linux", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 1, ljust("u", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 2, ljust("0.0.0", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 3, ljust("u", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 4, ljust("riscv64", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 5, ljust("riscv64", res, size, fillchar, '\0'));

    return 0;
}

int64_t syscall_discard(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    return -1;
}

void do_syscall(Cpu *cpu) {
    int64_t return_value = -1;
    int64_t syscall_num = cpu->regs[17];

    int64_t arg0 = cpu->regs[10];
    int64_t arg1 = cpu->regs[11];
    int64_t arg2 = cpu->regs[12];
    int64_t arg3 = cpu->regs[13];
    int64_t arg4 = cpu->regs[14];
    int64_t arg5 = cpu->regs[15];


    printf("syscall %ld\n", syscall_num);
    switch (syscall_num) {
        case 160:
            return_value = sys_uname(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;
        default:
            printf("Unknown syscall %ld\n", syscall_num);
            return_value = syscall_discard(arg0, arg1, arg2, arg3, arg4, arg5);
            break;
    }
    cpu->regs[10] = return_value;
}