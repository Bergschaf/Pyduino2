use crate::cpu::Cpu;
use crate::emulator::Emulator;
use crate::kernel::do_syscall;

pub fn sign_extend(val: u64, from: u8) -> i64 {
    let shift = 64 - from;
    ((val << shift) as i64 >> shift)
}

fn crop_to_32_and_sign_extend(val: i64) -> i64 {
    sign_extend(val as u64, 32) as i64
}

#[derive(Debug)]
pub struct RType {
    opcode: u8,
    rd: u8,
    funct3: u8,
    rs1: u8,
    rs2: u8,
    funct7: u8,
}

#[derive(Debug)]
pub struct IType {
    opcode: u8,
    rd: u8,
    funct3: u8,
    rs1: u8,
    imm: i32,
}

#[derive(Debug)]
pub struct SType {
    opcode: u8,
    imm: i32,
    funct3: u8,
    rs1: u8,
    rs2: u8,
}

#[derive(Debug)]
pub struct BType {
    opcode: u8,
    imm: i32,
    funct3: u8,
    rs1: u8,
    rs2: u8,
}

#[derive(Debug)]
pub struct UType {
    opcode: u8,
    rd: u8,
    imm: i32,
}

#[derive(Debug)]
pub struct JType {
    opcode: u8,
    rd: u8,
    imm: i32,
}

pub fn parse_r_type(inst: u64) -> RType {
    RType {
        opcode: (inst & 0b1111111) as u8,
        rd: ((inst >> 7) & 0b11111) as u8,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        rs2: ((inst >> 20) & 0b11111) as u8,
        funct7: ((inst >> 25) & 0b1111111) as u8,
    }
}

pub fn parse_i_type(inst: u64) -> IType {
    IType {
        opcode: (inst & 0b1111111) as u8,
        rd: ((inst >> 7) & 0b11111) as u8,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        imm: sign_extend(((inst >> 20) & 0b111111111111), 12) as i32,
    }
}

pub fn parse_s_type(inst: u64) -> SType {
    SType {
        opcode: (inst & 0b1111111) as u8,
        imm: sign_extend((((inst >> 7) & 0b11111) | ((inst >> 25) & 0b1111111) << 5), 12) as i32,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        rs2: ((inst >> 20) & 0b11111) as u8,
    }
}

pub fn parse_b_type(inst: u64) -> BType {
    BType {
        opcode: (inst & 0b1111111) as u8,
        imm: sign_extend((((inst >> 8) & 0b1111) | ((inst >> 25) & 0b111111) << 4 | ((inst >> 7) & 0b1) << 10 | ((inst >> 31) & 0b1) << 11) << 1, 13) as i32,
        funct3: ((inst >> 12) & 0b111) as u8,
        rs1: ((inst >> 15) & 0b11111) as u8,
        rs2: ((inst >> 20) & 0b11111) as u8,
    }
}

pub fn parse_u_type(inst: u64) -> UType {
    UType {
        opcode: (inst & 0b1111111) as u8,
        rd: ((inst >> 7) & 0b11111) as u8,
        imm: sign_extend(((inst >> 12) & 0b11111111111111111111) << 12, 32) as i32,
    }
}

pub fn parse_j_type(inst: u64) -> JType {
    JType {
        opcode: (inst & 0b1111111) as u8,
        rd: ((inst >> 7) & 0b11111) as u8,
        imm: sign_extend((((inst >> 21) & 0b1111111111) | ((inst >> 20) & 0b1) << 10 | ((inst >> 12) & 0b11111111) << 11 | ((inst >> 31) & 0b1) << 19) << 1, 21) as i32,
    }
}

#[derive(Debug)]
pub enum Instruction {
    LUI(UType),
    AUIPC(UType),
    JAL(JType),
    JALR(IType),
    BEQ(BType),
    BNE(BType),
    BLT(BType),
    BGE(BType),
    BLTU(BType),
    BGEU(BType),
    LB(IType),
    LH(IType),
    LW(IType),
    LD(IType),
    LWU(IType),
    LBU(IType),
    LHU(IType),
    SB(SType),
    SH(SType),
    SW(SType),
    SD(SType),
    ADDI(IType),
    SLTI(IType),
    SLTIU(IType),
    XORI(IType),
    ORI(IType),
    ANDI(IType),
    SLLI(IType),
    SRLI(IType),
    SRAI(IType),
    ADD(RType),
    SUB(RType),
    SLL(RType),
    SLT(RType),
    SLTU(RType),
    XOR(RType),
    SRL(RType),
    SRA(RType),
    OR(RType),
    AND(RType),
    FENCE,
    FENCE_I,
    ECALL,
    EBREAK,

    ADDIW(IType),
    SLLIW(IType),
    SRLIW(IType),
    SRAIW(IType),
    ADDW(RType),
    SUBW(RType),
    SLLW(RType),
    SRLW(RType),
    SRAW(RType),
}

pub fn decode_instruction(inst: u64) -> Instruction {
    let opcode = inst & 0b1111111;
    match opcode {
        0b0110111 => Instruction::LUI(parse_u_type(inst)),
        0b0010111 => Instruction::AUIPC(parse_u_type(inst)),
        0b1101111 => Instruction::JAL(parse_j_type(inst)),
        0b1100111 => Instruction::JALR(parse_i_type(inst)),
        0b1100011 => {
            let inst = parse_b_type(inst);
            match inst.funct3 {
                0b000 => Instruction::BEQ(inst),
                0b001 => Instruction::BNE(inst),
                0b100 => Instruction::BLT(inst),
                0b101 => Instruction::BGE(inst),
                0b110 => Instruction::BLTU(inst),
                0b111 => Instruction::BGEU(inst),
                _ => panic!("Unknown funct3 for B-type instruction"),
            }
        }
        0b0000011 => {
            let inst = parse_i_type(inst);
            match inst.funct3 {
                0b000 => Instruction::LB(inst),
                0b001 => Instruction::LH(inst),
                0b010 => Instruction::LW(inst),
                0b011 => Instruction::LD(inst),
                0b100 => Instruction::LBU(inst),
                0b101 => Instruction::LHU(inst),
                0b110 => Instruction::LWU(inst),
                _ => panic!("Unknown funct3 for I-type instruction"),
            }
        }
        0b0100011 => {
            let inst = parse_s_type(inst);
            match inst.funct3 {
                0b000 => Instruction::SB(inst),
                0b001 => Instruction::SH(inst),
                0b010 => Instruction::SW(inst),
                0b011 => Instruction::SD(inst),
                _ => panic!("Unknown funct3 for S-type instruction"),
            }
        }
        0b0010011 => {
            let inst = parse_i_type(inst);
            match inst.funct3 {
                0b000 => Instruction::ADDI(inst),
                0b010 => Instruction::SLTI(inst),
                0b011 => Instruction::SLTIU(inst),
                0b100 => Instruction::XORI(inst),
                0b101 => {
                    if (inst.imm >> 5) & 0b1111111 == 0 {
                        Instruction::SRLI(inst)
                    } else {
                        Instruction::SRAI(inst)
                    }
                }
                0b110 => Instruction::ORI(inst),
                0b001 => Instruction::SLLI(inst),
                0b111 => Instruction::ANDI(inst),
                _ => panic!("Unknown funct3 for I-type instruction"),
            }
        }
        0b0110011 => {
            let inst = parse_r_type(inst);
            match inst.funct3 {
                0b000 => {
                    if inst.funct7 == 0 {
                        Instruction::ADD(inst)
                    } else {
                        Instruction::SUB(inst)
                    }
                }
                0b001 => Instruction::SLL(inst),
                0b010 => Instruction::SLT(inst),
                0b011 => Instruction::SLTU(inst),
                0b100 => Instruction::XOR(inst),
                0b101 => {
                    if inst.funct7 == 0 {
                        Instruction::SRL(inst)
                    } else {
                        Instruction::SRA(inst)
                    }
                }
                0b110 => Instruction::OR(inst),
                0b111 => Instruction::AND(inst),
                _ => panic!("Unknown funct3 for R-type instruction"),
            }
        }
        0b0001111 => Instruction::FENCE,
        0b1110011 => {
            // TODO everything is ecall now
            if (inst >> 20) & 0xfff == 0 {
                Instruction::ECALL
            } else {
                panic!("Why is this not an ecall")
            }
        }
        0b0011011 => {
            let inst = parse_i_type(inst);
            match inst.funct3 {
                0b000 => Instruction::ADDIW(inst),
                0b001 => Instruction::SLLIW(inst),
                0b101 => {
                    if (inst.imm >> 5) & 0b1111111 == 0 {
                        Instruction::SRLIW(inst)
                    } else {
                        Instruction::SRAIW(inst)
                    }
                }
                _ => panic!("Unknown funct3 for I-type instruction"),
            }
        }
        0b0111011 => {
            let inst = parse_r_type(inst);
            match inst.funct3 {
                0b000 => {
                    if inst.funct7 == 0 {
                        Instruction::ADDW(inst)
                    } else {
                        Instruction::SUBW(inst)
                    }
                }
                0b001 => Instruction::SLLW(inst),
                0b101 => {
                    if inst.funct7 == 0 {
                        Instruction::SRLW(inst)
                    } else {
                        Instruction::SRAW(inst)
                    }
                }
                _ => panic!("Unknown funct3 for R-type instruction"),
            }
        }
        _ => panic!("Unknown opcode"),
    }
}


pub fn execute_instruction(inst: Instruction, emulator: &mut Emulator) {
    let cpu = &mut emulator.cpu;
    let memory = &mut emulator.memory;
    match inst {
        Instruction::AUIPC(u_type) => {
            cpu.registers[u_type.rd as usize] = cpu.pc + u_type.imm as i64;
        }
        Instruction::LUI(u_type) => {
            cpu.registers[u_type.rd as usize] = u_type.imm as i64;
        }
        Instruction::ADDI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] + i_type.imm as i64;
        }
        Instruction::ADDIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
        }
        Instruction::ADD(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] + cpu.registers[r_type.rs2 as usize];
        }
        Instruction::ADDW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] + cpu.registers[r_type.rs2 as usize]);
        }
        Instruction::SUB(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] - cpu.registers[r_type.rs2 as usize];
        }
        Instruction::SUBW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] - cpu.registers[r_type.rs2 as usize]);
        }
        Instruction::SLLI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] << (i_type.imm & 0b111111);
        }
        Instruction::SLLIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[i_type.rs1 as usize] << (i_type.imm & 0b111111));
        }
        Instruction::SLL(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] << (cpu.registers[r_type.rs2 as usize] & 0b111111);
        }
        Instruction::SLLW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] << (cpu.registers[r_type.rs2 as usize] & 0b111111));
        }
        Instruction::SRLI(i_type) => {
            cpu.registers[i_type.rd as usize] = (cpu.registers[i_type.rs1 as usize] as u64 >> (i_type.imm & 0b111111)) as i64;
        }
        Instruction::SRAI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] >> (i_type.imm & 0b111111);
        }
        Instruction::SRL(r_type) => {
            cpu.registers[r_type.rd as usize] = (cpu.registers[r_type.rs1 as usize] as u64 >> (cpu.registers[r_type.rs2 as usize] & 0b111111)) as i64;
        }
        Instruction::SRLW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend((cpu.registers[r_type.rs1 as usize] as u64 >> (cpu.registers[r_type.rs2 as usize] & 0b111111)) as i64);
        }
        Instruction::SRA(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] >> (cpu.registers[r_type.rs2 as usize] & 0b111111);
        }
        Instruction::SRAW(r_type) => {
            cpu.registers[r_type.rd as usize] = crop_to_32_and_sign_extend(cpu.registers[r_type.rs1 as usize] >> (cpu.registers[r_type.rs2 as usize] & 0b111111));
        }
        Instruction::SLTI(i_type) => {
            cpu.registers[i_type.rd as usize] = if cpu.registers[i_type.rs1 as usize] < i_type.imm as i64 { 1 } else { 0 };
        }
        Instruction::SLTIU(i_type) => {
            cpu.registers[i_type.rd as usize] = if (cpu.registers[i_type.rs1 as usize] as u64) < i_type.imm as u64 { 1 } else { 0 };
        }
        Instruction::XORI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] ^ (i_type.imm as i64);
        }
        Instruction::ORI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] | (i_type.imm as i64);
        }
        Instruction::ANDI(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.registers[i_type.rs1 as usize] & (i_type.imm as i64);
        }
        Instruction::SLT(r_type) => {
            cpu.registers[r_type.rd as usize] = if cpu.registers[r_type.rs1 as usize] < cpu.registers[r_type.rs2 as usize] { 1 } else { 0 };
        }
        Instruction::SLTU(r_type) => {
            cpu.registers[r_type.rd as usize] = if (cpu.registers[r_type.rs1 as usize] as u64) < cpu.registers[r_type.rs2 as usize] as u64 { 1 } else { 0 };
        }
        Instruction::XOR(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] ^ cpu.registers[r_type.rs2 as usize];
        }
        Instruction::OR(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] | cpu.registers[r_type.rs2 as usize];
        }
        Instruction::AND(r_type) => {
            cpu.registers[r_type.rd as usize] = cpu.registers[r_type.rs1 as usize] & cpu.registers[r_type.rs2 as usize];
        }
        Instruction::FENCE => {}
        Instruction::FENCE_I => {}
        Instruction::ECALL => {
            let syscall_number = cpu.registers[17];
            let args = [cpu.registers[10], cpu.registers[11], cpu.registers[12], cpu.registers[13], cpu.registers[14], cpu.registers[15]];
            do_syscall(emulator, syscall_number, args);
        }
        Instruction::EBREAK => {
            panic!("EBREAK");
        }
        Instruction::JAL(j_type) => {
            cpu.registers[j_type.rd as usize] = cpu.pc + 4;
            cpu.pc += j_type.imm as i64;
        }
        Instruction::JALR(i_type) => {
            cpu.registers[i_type.rd as usize] = cpu.pc + 4;
            cpu.pc = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64) & !1;
        }
        Instruction::BEQ(b_type) => {
            if cpu.registers[b_type.rs1 as usize] == cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BNE(b_type) => {
            if cpu.registers[b_type.rs1 as usize] != cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BLT(b_type) => {
            if cpu.registers[b_type.rs1 as usize] < cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BGE(b_type) => {
            if cpu.registers[b_type.rs1 as usize] >= cpu.registers[b_type.rs2 as usize] {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BLTU(b_type) => {
            if (cpu.registers[b_type.rs1 as usize] as u64) < cpu.registers[b_type.rs2 as usize] as u64 {
                cpu.pc += b_type.imm as i64;
            }
        }
        Instruction::BGEU(b_type) => {
            if (cpu.registers[b_type.rs1 as usize] as u64) >= cpu.registers[b_type.rs2 as usize] as u64 {
                cpu.pc += b_type.imm as i64;
            }
        }

        Instruction::LB(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 1);
            cpu.registers[i_type.rd as usize] = sign_extend(data[0] as u64, 8) as i64;
        }
        Instruction::LH(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 2);
            cpu.registers[i_type.rd as usize] = sign_extend(u16::from_le_bytes([data[0], data[1]]) as u64, 16);
        }
        Instruction::LW(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 4);
            cpu.registers[i_type.rd as usize] = sign_extend(u32::from_le_bytes([data[0], data[1], data[2], data[3]]) as u64, 32) as i64;
        }
        Instruction::LD(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 8);
            cpu.registers[i_type.rd as usize] = u64::from_le_bytes([data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]]) as i64;
        }
        Instruction::LBU(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 1);
            cpu.registers[i_type.rd as usize] = u64::from(data[0]) as i64;
        }
        Instruction::LHU(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 2);
            cpu.registers[i_type.rd as usize] = u16::from_le_bytes([data[0], data[1]]) as i64;
        }
        Instruction::LWU(i_type) => {
            let address = (cpu.registers[i_type.rs1 as usize] + i_type.imm as i64);
            let data = memory.read(address.try_into().unwrap(), 4);
            cpu.registers[i_type.rd as usize] = u32::from_le_bytes([data[0], data[1], data[2], data[3]]) as i64;
        }

        Instruction::SB(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u8;
            memory.write(address.try_into().unwrap(), &[data]);
        }
        Instruction::SH(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u16;
            memory.write(address.try_into().unwrap(), &data.to_le_bytes());
        }
        Instruction::SW(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u32;
            memory.write(address.try_into().unwrap(), &data.to_le_bytes());
        }
        Instruction::SD(s_type) => {
            let address = (cpu.registers[s_type.rs1 as usize] + s_type.imm as i64);
            let data = cpu.registers[s_type.rs2 as usize] as u64;
            memory.write(address.try_into().unwrap(), &data.to_le_bytes());
        }

        Instruction::SRLIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend(((cpu.registers[i_type.rs1 as usize] as u64) >> (i_type.imm & 0b111111)) as i64);
        }
        Instruction::SRAIW(i_type) => {
            cpu.registers[i_type.rd as usize] = crop_to_32_and_sign_extend((cpu.registers[i_type.rs1 as usize] >> (i_type.imm & 0b111111)) as i64);
        }
    }
}
