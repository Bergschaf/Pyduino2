use crate::emulator::Emulator;
use crate::filesystem;
use syscall::flag as syscall_flags;


pub enum Syscall {
    Openat,
    Write,
    Writev,
    Exit,
    Brk,
    Mmap,
    Unknown,
}

pub fn decode_syscall(syscall: i64) -> Syscall {
    match syscall {
        56 => Syscall::Openat,
        64 => Syscall::Write,
        66 => Syscall::Writev,
        93 => Syscall::Exit,
        214 => Syscall::Brk,
        222 => Syscall::Mmap,
        _ => Syscall::Unknown,
    }
}

pub fn check_flags(flags: i64, expected: i64) -> bool{
    flags & expected == expected
}

pub fn syscall_write(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    let fd = args[0];
    let buf = args[1];
    let count = args[2];
    let string = emulator.memory.read_string(buf as usize, count as usize);
    print!("{}", string);
    count
}

pub fn syscall_writev(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    let fd = args[0];
    let iov = args[1];
    let iovcnt = args[2];
    let mut written = 0;
    for i in 0..iovcnt {
        let iov_base = emulator.memory.read_u64((iov + i * 16) as usize);
        let iov_len = emulator.memory.read_u64((iov + i * 16 + 8) as usize);
        let string = emulator.memory.read_string(iov_base as usize, iov_len as usize);
        // print green
        print!("\x1b[1;32m{}\x1b[0m", string);
        written += string.len();
    };
    written as i64
}


pub fn syscall_exit(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    let code = args[0];
    print!("Program exited with code {}\n", code);
    std::process::exit(code as i32);
    0
}

pub fn syscall_openat(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    let dirfd = args[0];
    let pathname = args[1];
    let flags = args[2];
    let mode = args[3];
    let path = emulator.memory.read_string(pathname as usize, 256);
    // TODO ignored dirfd
    syscall_open(emulator, [pathname, flags, mode, 0, 0, 0])
}

pub fn syscall_open(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    let pathname = args[0];
    let flags = args[1];
    let mode = args[2];
    let path = emulator.memory.read_string(pathname as usize, 256);
    let access_mode = match flags & 3 {
        0 => filesystem::AccessMode::Read,
        1 => filesystem::AccessMode::Write,
        2 => filesystem::AccessMode::ReadWrite,
        _ => panic!("unknow acces mode"),
    };
    // TODO ignored rest of flags
    let fd = emulator.kernel.filesystem.open_file(&path, access_mode,
                                                  filesystem::FileCreationFlags { create: false },
                                                  filesystem::FileStatusFlags { append: false });
    fd as i64
}

pub fn syscall_brk(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    // TODO ignored brk
    0
}

pub fn syscall_mmap(emulator: &mut Emulator, args: [i64; 6]) -> i64 {
    let addr = args[0];
    let length = args[1];
    let prot = args[2];
    let flags = args[3];
    let fd = args[4];
    let offset = args[5];

    if (addr != 0) {
        panic!("mmap addr not null not supported");
    };
    if (prot != syscall_flags::PROT_NONE.bits() as i64) {
        panic!("mmap flags not PROT_NONE not supported");
    };

    if (flags != 0b100010) // Map private and anonymous
    {
        panic!("mmap flags are not supported");
    };

    let addr = emulator.memory.last_mmap - length;
    emulator.memory.last_mmap = addr;
    print!("mmap addr: {:x} length: {:x}\n", addr as usize, length);
    emulator.memory.write(addr as usize, &vec![0; length as usize]);
    addr
}