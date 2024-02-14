use crate::emulator::Emulator;
use crate::instructions;
use crate::instructions::Instruction;

pub struct Cpu {
    pub registers: [i64; 32], // TODO zero_register
    pub pc: i64,
}

impl Cpu {
    pub fn new() -> Cpu {
        Cpu {
            registers: [0; 32],
            pc: 0,
        }
    }
    pub fn next_instruction(emulator: &mut Emulator) -> Instruction {
        let instruction = emulator.memory.read_u64(emulator.cpu.pc as usize);
        let inst:Instruction = instructions::decode_instruction(instruction);
        //print!("Instruction: {:?}\n", inst);
        //print!("PC: 0x{:0x}\n", emulator.cpu.pc);
        return inst;
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
