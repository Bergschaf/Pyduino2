use crate::kernel;
use crate::memory;
use crate::cpu;
use crate::cpu::Cpu;
use crate::elfLoader;
use crate::pe32Loader;
use crate::instructions;

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
        self.cpu.pc = elf.entry_point as i64;

        let stack_adress = 0x1000000;
        let stack_size = 0x10000;
        self.memory.create_mapping(stack_size, stack_adress);
        self.cpu.registers[2] = stack_adress + stack_size - 0x1000;
    }

    pub fn load_pe32_file(&mut self, filename: &str) {
        let pe32 = pe32Loader::load_p32_file(filename);
        pe32.load_into_memory(&mut self.memory);
        self.cpu.pc = pe32.optional_header.address_of_entry_point as i64;
        print!("Entry point: 0x{:0x}\n", self.cpu.pc);
        //self.cpu.registers[2] = memory::MEMORY_SIZE - memory::STACK_SIZE;
    }

    pub fn run(&mut self) {
        loop {
            let inst = Cpu::next_instruction(self);
            let prev_pc = self.cpu.pc;
            instructions::execute_instruction(inst, self);
            self.cpu.registers[0] = 0;
            if self.cpu.pc == prev_pc {
                self.cpu.pc += 4;
            }
            //self.cpu.print_registers();
        }
    }
}
