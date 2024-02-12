use log::debug;

mod emulator;
mod kernel;
mod memory;
mod cpu;
mod filesystem;
mod instructions;
mod elfLoader;

fn main() {
    env_logger::init();

    let mut emu = emulator::Emulator::new();
    emu.load_elf_file("../bin/hello_world");
    emu.run();
}
