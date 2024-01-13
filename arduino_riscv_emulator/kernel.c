//
// Created by bergschaf on 1/8/24.
//

#include "kernel.h"
// import usleep
#include <unistd.h>

int64_t sys_uname(Cpu *cpu, int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    int size = 65;
    char fillchar = '0';

    char *res = malloc(size);
    memory_puts(cpu, arg0, ljust("Linux", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 1, ljust("u", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 2, ljust("6.6.8", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 3, ljust("u", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 4, ljust("riscv64", res, size, fillchar, '\0'));
    memory_puts(cpu, arg0 + size * 5, ljust("u", res, size, fillchar, '\0'));

    return 0;
}

int64_t sys_set_tid(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    return 69;
}

int64_t syscall_discard(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    return -1;
}

int64_t sys_exit(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    printf("exit(%ld)\n", arg0);
    exit(arg0);
}

int64_t sys_writev(Cpu *cpu, int64_t arg0, int64_t addr, int64_t count, int64_t arg3, int64_t arg4, int64_t arg5) {
    // treat as print TODO change this
    int res = 0;
    for (int i = 0; i < count; ++i) {
        int64_t base = memory_loadw(cpu, addr + i * 16);
        int64_t len = memory_loadw(cpu, addr + i * 16 + 8);
        //printf("base: %ld, len: %ld\n", base, len);
        char *str = malloc(len);
        memory_loads(cpu, base, str, len);
        // print colored
        printf("\033[0;32m");
        printf("%s", str);
        printf("\033[0m");
        res += len;
        free(str);
    }
    return res;
}

int64_t sys_nanosleep(Cpu *cpu, int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    int64_t sleep = memory_loaddw(cpu, arg0 + 8) / 1000;
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("sleep %ld\n", sleep);
        printf("\033[0m");
    }
    // sleep
    usleep(sleep);
    return 0;
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

    if (LOG_LEVEL <= 3) {
        // print yellow
        printf("\033[0;33m");
        printf("syscall %ld\n", syscall_num);
        printf("\033[0m");
    }
    switch (syscall_num) {
        case 66:
            return_value = sys_writev(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 93:
            return_value = sys_exit(arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 96:
            return_value = sys_set_tid(arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 101:
            return_value = sys_nanosleep(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 160:
            return_value = sys_uname(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;
        default:
            // print red unknown syscall
            printf("\033[0;31m");
            printf("unknown syscall %ld\n", syscall_num);
            printf("\033[0m");
            return_value = syscall_discard(arg0, arg1, arg2, arg3, arg4, arg5);
            break;
    }
    cpu->regs[10] = return_value;
}