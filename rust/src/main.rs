use std::process::exit;
use log::debug;

mod emulator;
mod kernel;
mod memory;
mod cpu;
mod filesystem;
mod instructions;
mod elfLoader;
mod syscall;

fn main() {
    // todo remove usize for arduino
    env_logger::init();
    let mut emu = emulator::Emulator::new();
    emu.load_elf_file("../bin/test_read.o");
    emu.run();
}
