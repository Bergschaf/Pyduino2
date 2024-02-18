use crate::emulator::Emulator;
use crate::instructions;
use crate::instructions::Instruction;

pub struct Cpu {
    pub registers: [i64; 32], // TODO zero_register
    pub csr: [i64; 4096],
    pub pc: i64,
}

impl Cpu {
    pub fn new() -> Cpu {
        Cpu {
            registers: [0; 32],
            csr: [0; 4096],
            pc: 0,
        }
    }
    pub fn next_instruction(emulator: &mut Emulator) -> (Instruction, bool){
        let instruction = emulator.memory.read_u64(emulator.cpu.pc as usize);
        let inst_comp = instructions::decode_compressed_instruction(instruction);
        match inst_comp {
            Some(inst) => {
                (inst, true)
            },
            None => {
                let inst = instructions::decode_instruction(instruction);
                (inst, false)
            }
        }
    }

    pub fn print_registers(&self) {
        for i in 0..32 {
            if self.registers[i] == 0 {
                continue;
            }
            print!("    x{}: 0x{:0x}\n", i, self.registers[i]);
        }
    }
}
