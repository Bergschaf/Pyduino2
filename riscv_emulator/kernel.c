//
// Created by bergschaf on 1/8/24.
//

#include "kernel.h"
#include "filesystem.h"
// import usleep
#include <unistd.h>
// mmap flags
#include <sys/mman.h>
#include <utils.h>
#include <time.h>

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
    exit(113);
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

int64_t sys_write(Cpu *cpu, int64_t fd, int64_t buf, int64_t count, int64_t arg3, int64_t arg4, int64_t arg5) {
    // treat as print
    if (fd == 1 || fd == 2) { // TODO fd change
        // print
        count++;
        char *str = malloc(count);
        memory_loads(cpu, buf, str, count);
        // print colored
        printf("\033[0;32m");
        printf("%s", str);
        printf("\033[0m");
        free(str);
    } else {
        // debug
        char *str = malloc(count);
        memory_loads(cpu, buf, str, count);
        printf("Write to fd %ld: ", fd);
        printf("%s\n", str);
        free(str);
    }
    return count;
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

int64_t sys_getrandom(Cpu *cpu, int64_t buf, int64_t buflen, int64_t flags, int64_t arg3, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("getrandom(%ld, %ld, %ld)\n", buf, buflen, flags);
        printf("\033[0m");
    }
    // fill with random
    for (int i = 0; i < buflen; ++i) {
        cpu->mem[buf + i] = rand() % 256;
    }
    return buflen;
}

int64_t sys_openat(Cpu *cpu, int64_t dirfd, int64_t pathname, int64_t flags, int64_t mode, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("openat(%ld, %ld, %ld, %ld)\n", dirfd, pathname, flags, mode);
        // print the path
        char *path = malloc(256);
        memory_loads(cpu, pathname, path, 256);
        printf("path: %s\n", path);
        printf("\033[0m");
    }
    // get path
    char *path = malloc(256);
    memory_loads(cpu, pathname, path, 256);
    // open file
    int fd = open_file(path, flags, mode);
    free(path);
    return fd;
}

int64_t sys_read(Cpu *cpu, int64_t fd, int64_t buf, int64_t count, int64_t arg3, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("read(%ld, %ld, %ld)\n", fd, buf, count);
        printf("\033[0m");
    }
    // read
    return read_file(fd, (char *) cpu->mem + buf, count);
}

int64_t sys_close(Cpu *cpu, int64_t fd, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("close(%ld)\n", fd);
        printf("\033[0m");
    }
    // close
    int res = close_file(fd);
    printf("close(%ld) = %d\n", fd, res);
    return res;
}

int64_t sys_lseek(Cpu *cpu, int64_t fd, int64_t offset, int64_t whence, int64_t arg3, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("lseek(%ld, %ld, %ld)\n", fd, offset, whence);
        printf("\033[0m");
    }
    // lseek
    return lseek_file(fd, offset, whence);
}

int64_t sys_ioctl(Cpu *cpu, int64_t fd, int64_t request, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("ioctl(%ld, %ld)\n", fd, request);
        printf("\033[0m");
    }
    return 0;
}

int64_t sys_sbrk(Cpu *cpu, int64_t addr, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("brk(0x%lx)\n", addr);
        printf("\033[0m");
    }
    if (addr == 0) {
        printf("current break: 0x%lx\n", cpu->curr_break);
        return cpu->curr_break;
    } else {
        cpu->curr_break += addr;
        printf("new break: 0x%lx\n", cpu->curr_break);
        return cpu->curr_break - addr;
    }
}

int64_t sys_clock_gettime(Cpu *cpu, int64_t clk_id, int64_t tp) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("clock_gettime(%ld, %ld)\n", clk_id, tp);
        printf("\033[0m");
    }
    long seconds = 12345;
    long nanoseconds = 67890;
    cpu->mem[tp] = seconds;
    //memory_loaddw(cpu, tp + 8) = nanoseconds;
    return 0;
}

int64_t sys_mmap(Cpu *cpu, int64_t addr, int64_t len, int64_t prot, int64_t flags, int64_t fd, int64_t offset) {
    // print colored
    if (LOG_LEVEL <= 3) {
        printf("\033[0;33m");
        printf("mmap(%ld, %ld, %ld, 0b%lb, %ld, %ld)\n", addr, len, prot, flags, fd, offset);
        printf("\033[0m");
    }
    if (check_flag(flags, MAP_ANONYMOUS) && check_flag(flags, MAP_PRIVATE)) {
        // allocate memory
        if (addr != 0){
            // print red
            printf("\033[0;31m");
            printf("mmap(%ld, %ld, %ld, 0b%lb, %ld, %ld)\n", addr, len, prot, flags, fd, offset);
            printf("mmap addr not supported\n");
            printf("\033[0m");
            exit(42);
            return -1;
        }
        return memory_mmap_anonymous(cpu, len);

    } else {
        // print red
        printf("\033[0;31m");
        printf("mmap(%ld, %ld, %ld, 0b%lb, %ld, %ld)\n", addr, len, prot, flags, fd, offset);
        printf("mmap flags not supported\n");
        printf("\033[0m");
        exit(42);
        return -1;
    }
    return 0;
}

void initialize_kernel() {
    initialize_filesystem();
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
        case 29:
            return_value = sys_ioctl(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 56:
            return_value = sys_openat(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 57:
            return_value = sys_close(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 62:
            return_value = sys_lseek(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 63:
            return_value = sys_read(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 64:
            return_value = sys_write(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

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

        case 113:
            return_value = sys_clock_gettime(cpu,arg0, arg1);
            break;

        case 160:
            return_value = sys_uname(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 214:
            return_value = sys_sbrk(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 222:
            return_value = sys_mmap(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
            break;

        case 278:
            return_value = sys_getrandom(cpu, arg0, arg1, arg2, arg3, arg4, arg5);
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