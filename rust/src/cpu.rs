use crate::emulator::Emulator;
use crate::instructions;
use crate::instructions::Instruction;

pub struct Cpu {
    pub registers: [u64; 32],
    pub pc: u64,
}

impl Cpu {
    pub fn new() -> Cpu {
        Cpu {
            registers: [0; 32],
            pc: 0,
        }
    }
    pub fn next_instruction(emulator: &mut Emulator) {
        let instruction = emulator.memory.read_u64(emulator.cpu.pc as usize);
        let inst:Instruction = instructions::decode_instruction(instruction);
        print!("Instruction: {:?}\n", inst);



        emulator.cpu.pc += 4;
    }
}
