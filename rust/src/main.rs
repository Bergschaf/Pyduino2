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
mod pe32Loader;

fn main() {
    // todo remove usize for arduino
    env_logger::init();
    //pe32Loader::load_p32_file("../riscv_interpreter/linux-kernel/Image".into()); /panic!("done");


    let mut emu = emulator::Emulator::new();
    //emu.load_elf_file("../bin/vmlinux");
    emu.load_pe32_file("../bin/Image");
    emu.run();
}
