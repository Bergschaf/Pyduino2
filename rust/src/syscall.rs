use crate::emulator::Emulator;

pub enum Syscall {
    Write = 64,
    Writev = 66,
    Exit = 93,
    Unknown = -1,
}

pub fn decode_syscall(syscall: i64) -> Syscall {
    match syscall {
        64 => Syscall::Write,
        66 => Syscall::Writev,
        93 => Syscall::Exit,
        _ => Syscall::Unknown,
    }
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

