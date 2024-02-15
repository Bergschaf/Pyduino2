use crate::emulator::Emulator;
use crate::filesystem::Filesystem;
use crate::syscall::*;

pub struct Kernel {
    pub filesystem: Filesystem,
}


impl Kernel {
    pub fn new() -> Kernel {
        Kernel {
            filesystem: Filesystem::new(),
        }
    }
}

pub fn do_syscall(emulator: &mut Emulator, syscall_num: i64, args: [i64; 6]){
    let syscall = decode_syscall(syscall_num);
    // print colored syscall number
    print!("\x1b[1;31mSyscall: {}\x1b[0m\n", syscall_num);
    let return_value =  match syscall {
        Syscall::Write => {
            syscall_write(emulator, args)
        },
        Syscall::Writev => {
            syscall_writev(emulator, args)
        },
        Syscall::Exit => {
            syscall_exit(emulator, args)
        },
        Syscall::Openat => {
            syscall_openat(emulator, args)
        },
        Syscall::Brk => {
            syscall_brk(emulator, args)
        },
        Syscall::Mmap => {
            syscall_mmap(emulator, args)
        },
        Syscall::Unknown => {
            print!("Unknown syscall {}\n", syscall_num);
            -1
        },
    };
    print!("Return value: {}\n", return_value);
    emulator.cpu.registers[10] = return_value;
}


