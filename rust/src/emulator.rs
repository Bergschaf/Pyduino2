use crate::kernel;
use crate::memory;
use crate::cpu;
use crate::cpu::Cpu;
use crate::elfLoader;

pub struct Emulator {
    pub kernel: kernel::Kernel,
    pub memory: memory::Memory,
    pub cpu: cpu::Cpu,
}

impl Emulator {
    pub fn new() -> Emulator {
        Emulator {
            kernel: kernel::Kernel::new(),
            memory: memory::Memory::new(),
            cpu: cpu::Cpu::new(),
        }
    }

    pub fn load_elf_file(&mut self, filename: &str) {
        let elf = elfLoader::load_elf_file(filename);
        elf.load_into_memory(&mut self.memory);
        self.cpu.pc = elf.entry_point as u64;
        self.cpu.registers[2] = memory::MEMORY_SIZE as u64 - memory::STACK_SIZE as u64;
    }

    pub fn run(&mut self) {
        loop {
            Cpu::next_instruction(self);
        }
    }
}
