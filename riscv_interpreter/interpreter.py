XLEN = 64
from termcolor import colored


def int_from_bin(a: int, word_size=XLEN):
    # twos complement
    if a & (1 << (word_size - 1)):
        a = a - (1 << word_size)
    return a


def int_to_bin(a: int, word_size=XLEN):
    # twos complement
    if a < 0:
        a = (1 << word_size) + a
    return a


def se(value, bits=XLEN):
    sign_bit = 1 << (bits - 1)
    return (value & (sign_bit - 1)) - (value & sign_bit)


def sign_extend(value, curr, bits=XLEN):
    if curr >= bits:
        return value
    sign_bit = 1 << (curr - 1)
    return int_to_bin((value & (sign_bit - 1)) - (value & sign_bit))


class Registers:
    NAMES = {
        0: "zero",
        1: "ra",
        2: "sp",
        3: "gp",
        4: "tp",
        5: "t0",
        6: "t1",
        7: "t2",
        8: "s0/fp",
        9: "s1",
        10: "a0",
        11: "a1",
        12: "a2",
        13: "a3",
        14: "a4",
        15: "a5",
        16: "a6",
        17: "a7",
        18: "s2",
        19: "s3",
        20: "s4",
        21: "s5",
        22: "s6",
        23: "s7",
        24: "s8",
        25: "s9",
        26: "s10",
        27: "s11",
        28: "t3",
        29: "t4",
        30: "t5",
        31: "t6",
    }

    VALUES = {v: k for k, v in NAMES.items()}

    def __init__(self):
        self.registers = [0] * 32
        self.pc = 0

    def __getitem__(self, item):
        return self.registers[item]

    def __setitem__(self, key, value):
        if key == 0:
            return
        value = value & pow(2, XLEN) - 1  # TODO Change to 64 bits
        self.registers[key] = value

    def __repr__(self):
        return "\n".join(
            str(x) + ": " + str(int_from_bin(self[x])) + f" | {hex(self[x])} ({self.NAMES[x]})" for x in range(32) if
            self[x])


class InstructionType:
    def __init__(self, instruction: int):
        self.inst = instruction
        self.opcode = instruction & 0b111111
        self.rd = None
        self.rs1 = None
        self.rs2 = None
        self.imm = None
        self.imm_left = None  # The immediate of the S and B type (in the place of the rd value)
        self.funct3 = None
        self.funct7 = None

    def __repr__(self):
        # print all attributes of the class
        return f"""Instruction:
        opcode: {bin(self.opcode)}
        rd: {self.rd} | {Registers.NAMES[self.rd] if self.rd is not None else None}
        rs1: {self.rs1} | {Registers.NAMES[self.rs1] if self.rs1 is not None else None}
        rs2: {self.rs2} | {Registers.NAMES[self.rs2] if self.rs2 is not None else None}
        imm: {int_from_bin(self.imm) if self.imm is not None else None} | {bin(self.imm) if self.imm is not None else None}
        funct3: {bin(self.funct3) if self.funct3 is not None else None}
        funct7: {bin(self.funct7) if self.funct7 is not None else None}
        """


class RType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.rs2 = (self.inst >> 20) & 0b11111
        self.funct7 = self.inst >> 25
        return self


class SType(InstructionType):
    def decode(self):
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.rs2 = (self.inst >> 20) & 0b11111

        imm40 = (self.inst >> 7) & 0b11111
        imm115 = (self.inst >> 25) & 0b1111111
        self.imm = sign_extend((imm115 << 5) | imm40, 12)
        return self


class IType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.imm = sign_extend(self.inst >> 20, 12)
        return self


class UType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        self.imm = sign_extend(self.inst & 0xfffff000,32)
        return self


class JType(InstructionType):
    def decode(self):
        self.rd = (self.inst >> 7) & 0b11111
        imm = (self.inst >> 12) & 0xfffff
        imm20 = (imm >> 19) & 1
        imm101 = (imm >> 9) & 0b1111111111
        imm11 = (imm >> 8) & 1
        imm1912 = imm & 0b11111111
        self.imm = (imm20 << 20) | (imm1912 << 12) | (imm11 << 11) | (imm101 << 1)
        self.imm = sign_extend(self.imm, 21)
        return self


class BType(InstructionType):
    def decode(self):
        imm4111 = (self.inst >> 7) & 0b11111
        self.funct3 = (self.inst >> 12) & 0b111
        self.rs1 = (self.inst >> 15) & 0b11111
        self.rs2 = (self.inst >> 20) & 0b11111
        imm121015 = (self.inst >> 25) & 0b111111
        imm12 = (imm121015 & 0b1000000) >> 6
        imm105 = imm121015 & 0b111111
        imm41 = (imm4111 & 0b11110) >> 1
        imm11 = imm4111 & 0b1
        imm = (imm12 << 12) | (imm11 << 11) | (imm105 << 5) | (imm41 << 1)
        self.imm = sign_extend(imm, 12)
        return self


class Instruction:
    def __init__(self, values: InstructionType, func):
        self.values = values
        self.func = func

    def do(self, kernel):
        return self.func(self.values, kernel)

    def __repr__(self):
        return str(self.values) + "\n" + self.func.__name__ + "\n\n"

class Instructions_A:
    @staticmethod
    def amo_swap_d(inst: RType, kernel):
        print("ammoswap: ", hex(kernel.registers.pc))
        addr = int_from_bin(kernel.registers[inst.rs1])
        val = kernel.registers[inst.rs2]
        kernel.registers[inst.rd] = kernel.memory.load_doubleword(addr)
        kernel.memory.store_doubleword(addr, val)

    @staticmethod
    def amo_swap_w(inst: RType, kernel):
        print("ammoswap: ", hex(kernel.registers.pc))
        addr = int_from_bin(kernel.registers[inst.rs1])
        val = kernel.registers[inst.rs2]
        kernel.registers[inst.rd] = sign_extend(kernel.memory.load_word(addr), 32)
        kernel.memory.store_word(addr, val)
        print(val, addr)

    @staticmethod
    def lr_w(inst: RType, kernel):
        print("amolr: ", hex(kernel.registers.pc))
        print(inst.funct7 & 0b11)
        addr = int_from_bin(kernel.registers[inst.rs1])
        kernel.registers[inst.rd] = sign_extend(kernel.memory.load_word(addr), 32)
        kernel.memory.reservations.add(addr)

    @staticmethod
    def sc_w(inst: RType, kernel):
        # never fails
        print("amosc: ", hex(kernel.registers.pc))
        addr = int_from_bin(kernel.registers[inst.rs1])
        if addr not in kernel.memory.reservations:
            print("invalud")
            exit()
        kernel.memory.store_word(addr, kernel.registers[inst.rs2])
        kernel.registers[inst.rd] = 0


    @staticmethod
    def decode(inst):
        opcode = inst & 0b1111111
        if opcode != 0b0101111:
            return False
        inst = RType(inst).decode()
        match inst.funct3:
            case 0b011:
                # RV64A
                func5 = inst.funct7 >> 2
                match func5:
                    case 0b00010:
                        # LR.D
                        return Instruction(inst, Instructions_A.lr_d)
                    case 0b00011:
                        # SC.D
                        return Instruction(inst, Instructions_A.sc_d)
                    case 0b00001:
                        # AMOSWAP.D
                        return Instruction(inst, Instructions_A.amo_swap_d)
                    case 0b00000:
                        # AMOADD.D
                        return Instruction(inst, Instructions_A.amo_add_d)
                    case 0b00100:
                        # AMOXOR.D
                        return Instruction(inst, Instructions_A.amo_xor_d)
                    case 0b01100:
                        # AMOAND.D
                        return Instruction(inst, Instructions_A.amo_and_d)
                    case 0b01000:
                        # AMOOR.D
                        return Instruction(inst, Instructions_A.amo_or_d)
                    case 0b10000:
                        # AMOMIN.D
                        return Instruction(inst, Instructions_A.amo_min_d)
                    case 0b10100:
                        # AMOMAX.D
                        return Instruction(inst, Instructions_A.amo_max_d)
                    case 0b11000:
                        # AMOMINU.D
                        return Instruction(inst, Instructions_A.amo_minu_d)
                    case 0b11100:
                        # AMOMAXU.D
                        return Instruction(inst, Instructions_A.amo_maxu_d)
                    case _:
                        raise Exception("Unknown funct5")



            case 0b010:
                # RV32A
                func5 = inst.funct7 >> 2
                match func5:
                    case 0b00010:
                        # LR.W
                        return Instruction(inst, Instructions_A.lr_w)
                    case 0b00011:
                        # SC.W
                        return Instruction(inst, Instructions_A.sc_w)
                    case 0b00001:
                        # AMOSWAP.W
                        return Instruction(inst, Instructions_A.amo_swap_w)
                    case 0b00000:
                        # AMOADD.W
                        return Instruction(inst, Instructions_A.amo_add_w)
                    case 0b00100:
                        # AMOXOR.W
                        return Instruction(inst, Instructions_A.amo_xor_w)
                    case 0b01100:
                        # AMOAND.W
                        return Instruction(inst, Instructions_A.amo_and_w)
                    case 0b01000:
                        # AMOOR.W
                        return Instruction(inst, Instructions_A.amo_or_w)
                    case 0b10000:
                        # AMOMIN.W
                        return Instruction(inst, Instructions_A.amo_min_w)
                    case 0b10100:
                        # AMOMAX.W
                        return Instruction(inst, Instructions_A.amo_max_w)
                    case 0b11000:
                        # AMOMINU.W
                        return Instruction(inst, Instructions_A.amo_minu_w)
                    case 0b11100:
                        # AMOMAXU.W
                        return Instruction(inst, Instructions_A.amo_maxu_w)

        print(inst)
        return False


class Instructions:
    CACHE = {}
    @staticmethod
    def addi(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] + inst.imm

    @staticmethod
    def slli(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] << inst.imm

    @staticmethod
    def srli(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] >> inst.imm

    @staticmethod
    def srai(inst: IType, kernel):
        sham = inst.imm & 0b11111
        if kernel.registers[inst.rs1] & 2 ** (XLEN - 1) != 0:  # MSB is 1, i.e. x is negative
            filler = int('1' * sham + '0' * (XLEN - sham), 2)
            kernel.registers[inst.rd] = (kernel.registers[inst.rs1] >> sham) | filler  # fill in 0's with 1's
        else:
            kernel.registers[inst.rd] = kernel.registers[inst.rs1] >> sham

    @staticmethod
    def slti(inst: IType, kernel):
        # TODO Test
        if int_from_bin(kernel.registers[inst.rs1]) < inst.imm:
            kernel.registers[inst.rd] = 1
        else:
            kernel.registers[inst.rd] = 0

    @staticmethod
    def sltiu(inst: IType, kernel):
        if kernel.registers[inst.rs1] < inst.imm:
            kernel.registers[inst.rd] = 1
        else:
            kernel.registers[inst.rd] = 0

    @staticmethod
    def andi(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] & inst.imm

    @staticmethod
    def ori(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] | inst.imm

    @staticmethod
    def xori(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] ^ inst.imm

    @staticmethod
    def lui(inst: IType, kernel):
        kernel.registers[inst.rd] = inst.imm

    @staticmethod
    def auipc(inst: IType, kernel):
        kernel.registers[inst.rd] = int_to_bin(kernel.registers.pc) + inst.imm

    @staticmethod
    def add(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] + kernel.registers[inst.rs2]

    @staticmethod
    def sub(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] - kernel.registers[inst.rs2]
        # maybe other way round

    @staticmethod
    def slt(inst: IType, kernel):
        if int_from_bin(kernel.registers[inst.rs1]) < int_from_bin(kernel.registers[inst.rs2]):
            kernel.registers[inst.rd] = 1
        else:
            kernel.registers[inst.rd] = 0

    @staticmethod
    def sltu(inst: IType, kernel):
        if kernel.registers[inst.rs1] < kernel.registers[inst.rs2]:
            kernel.registers[inst.rd] = 1
        else:
            kernel.registers[inst.rd] = 0

    @staticmethod
    def _or(inst: IType, kernel):
        # underscore weil sonst python mault
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] | kernel.registers[inst.rs2]

    @staticmethod
    def _and(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] & kernel.registers[inst.rs2]

    @staticmethod
    def xor(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] ^ kernel.registers[inst.rs2]

    @staticmethod
    def sll(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] << (kernel.registers[inst.rs2] & 0b111111)

    @staticmethod
    def srl(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.registers[inst.rs1] >> (kernel.registers[inst.rs2] & 0b111111)

    @staticmethod
    def sra(inst: IType, kernel):
        # https://stackoverflow.com/questions/64963170/how-to-do-arithmetic-right-shift-in-python-for-signed-and-unsigned-values

        sham = kernel.registers[inst.rs2] & 0b111111
        if kernel.registers[inst.rs1] & 2 ** (XLEN - 1) != 0:  # MSB is 1, i.e. x is negative
            filler = int('1' * sham + '0' * (XLEN - sham), 2)
            kernel.registers[inst.rd] = (kernel.registers[inst.rs1] >> sham) | filler  # fill in 0's with 1's
        else:
            kernel.registers[inst.rd] = kernel.registers[inst.rs1] >> sham

    @staticmethod
    def jal(inst: IType, kernel):
        target_adress = int_from_bin(inst.imm) + kernel.registers.pc
        kernel.registers[inst.rd] = kernel.registers.pc + 4
        kernel.registers.pc = target_adress

    @staticmethod
    def lw(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend(
            kernel.memory.load_word(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm)), 32)

    @staticmethod
    def lb(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend(
            kernel.memory.load_byte(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm)), 8)

    @staticmethod
    def lh(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend(
            kernel.memory.load_halfword(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm)), 16)

    @staticmethod
    def lbu(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.memory.load_byte(
            int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm))

    @staticmethod
    def lhu(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.memory.load_halfword(
            int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm))

    @staticmethod
    def lwu(inst: IType, kernel):
        kernel.registers[inst.rd] = kernel.memory.load_word(
            int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm))

    @staticmethod
    def ld(inst: IType, kernel):
        #print("Addr: ",int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm), hex(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm)))
        #print(kernel.memory.load_bytes(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm)-20,40))
        kernel.registers[inst.rd] = kernel.memory.load_doubleword(
            int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm))
        kernel.breakpoint()

    @staticmethod
    def sb(inst: IType, kernel):
        kernel.memory.store_byte(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm),
                                 kernel.registers[inst.rs2] & 0xff)

    @staticmethod
    def sh(inst: IType, kernel):
        kernel.memory.store_halfword(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm),
                                     kernel.registers[inst.rs2] & 0xffff)

    @staticmethod
    def sw(inst: IType, kernel):
        kernel.memory.store_word(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm),
                                 kernel.registers[inst.rs2] & 0xffffffff)

    @staticmethod
    def sd(inst: IType, kernel):
        # print(kernel.registers)
        # print(int_from_bin(kernel.registers[inst.rs1]))
        # print(inst.rs1)
        # print(inst.imm)
        kernel.memory.store_doubleword(int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm),
                                       kernel.registers[inst.rs2])

    @staticmethod
    def beq(inst: IType, kernel):
        if kernel.registers[inst.rs1] == kernel.registers[inst.rs2]:
            kernel.registers.pc += int_from_bin(inst.imm)

    @staticmethod
    def bne(inst: IType, kernel):
        if kernel.registers[inst.rs1] != kernel.registers[inst.rs2]:
            kernel.registers.pc += int_from_bin(inst.imm)

    @staticmethod
    def blt(inst: IType, kernel):
        if int_from_bin(kernel.registers[inst.rs1]) < int_from_bin(kernel.registers[inst.rs2]):
            kernel.registers.pc += int_from_bin(inst.imm)

    @staticmethod
    def bltu(inst: IType, kernel):
        if kernel.registers[inst.rs1] < kernel.registers[inst.rs2]:
            kernel.registers.pc += int_from_bin(inst.imm)

    @staticmethod
    def bge(inst: IType, kernel):
        if int_from_bin(kernel.registers[inst.rs1]) >= int_from_bin(kernel.registers[inst.rs2]):
            kernel.registers.pc += int_from_bin(inst.imm)

    @staticmethod
    def bgeu(inst: IType, kernel):
        if kernel.registers[inst.rs1] >= kernel.registers[inst.rs2]:
            kernel.registers.pc += int_from_bin(inst.imm)

    @staticmethod
    def addw(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend(int_to_bin(
            int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(kernel.registers[inst.rs2])) & 0xffffffff, 32)

    @staticmethod
    def subw(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend(int_to_bin(
            int_from_bin(kernel.registers[inst.rs1]) - int_from_bin(kernel.registers[inst.rs2])) & 0xffffffff, 32)

    @staticmethod
    def sllw(inst: IType, kernel):
        kernel.registers[inst.rd] = (kernel.registers[inst.rs1] << (kernel.registers[inst.rs2] & 0b11111)) & 0xffffffff

    @staticmethod
    def srlw(inst: IType, kernel):
        kernel.registers[inst.rd] = (kernel.registers[inst.rs1] & 0xffffffff) >> (kernel.registers[inst.rs2] & 0b111111)

    @staticmethod
    def sraw(inst: IType, kernel):
        # https://stackoverflow.com/questions/64963170/how-to-do-arithmetic-right-shift-in-python-for-signed-and-unsigned-values

        sham = kernel.registers[inst.rs2] & 0b111111
        kernel.registers[inst.rs1] &= 0xffffffff
        if kernel.registers[inst.rs1] & 2 ** (XLEN - 1) != 0:  # MSB is 1, i.e. x is negative
            filler = int('1' * sham + '0' * (XLEN - sham), 2)
            kernel.registers[inst.rd] = (kernel.registers[inst.rs1] >> sham) | filler  # fill in 0's with 1's
        else:
            kernel.registers[inst.rd] = kernel.registers[inst.rs1] >> sham

    @staticmethod
    def addiw(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend((kernel.registers[inst.rs1] & 0xffffffff) + inst.imm, 32)

    @staticmethod
    def slliw(inst: IType, kernel):
        kernel.registers[inst.rd] = sign_extend((kernel.registers[inst.rs1] & 0xffffffff) << (inst.imm & 0b11111), 32)

    @staticmethod
    def srliw(inst: IType, kernel):
        kernel.registers[inst.rd] = (kernel.registers[inst.rs1] & 0xffffffff) >> (inst.imm & 0b11111)

    @staticmethod
    def sraiw(inst: IType, kernel):
        sham = inst.imm & 0b11111
        kernel.registers[inst.rs1] &= 0xffffffff
        if kernel.registers[inst.rs1] & 2 ** (XLEN - 1) != 0:  # MSB is 1, i.e. x is negative
            filler = int('1' * sham + '0' * (XLEN - sham), 2)
            kernel.registers[inst.rd] = (kernel.registers[inst.rs1] >> sham) | filler  # fill in 0's with 1's
        else:
            kernel.registers[inst.rd] = kernel.registers[inst.rs1] >> sham

    @staticmethod
    def jalr(inst: IType, kernel):
        target_adress = int_from_bin(kernel.registers[inst.rs1]) + int_from_bin(inst.imm)
        target_adress = target_adress >> 1 << 1
        kernel.registers[inst.rd] = int_to_bin(kernel.registers.pc + 4)
        kernel.registers.pc = target_adress

    @staticmethod
    def ecall(inst: IType, kernel):
        return kernel.do_syscalL()

    @staticmethod
    def ebreak(inst: IType, kernel):
        return True

    @staticmethod
    def fence(inst: IType, kernel):
        print("fence: ", hex(kernel.registers.pc))
        pass

    @staticmethod
    def xxx(inst: IType, kernel):
        pass

    @staticmethod
    def xxx(inst: IType, kernel):
        pass

    @staticmethod
    def xxx(inst: IType, kernel):
        pass

    @staticmethod
    def decode(inst, kernel):
        if kernel.registers.pc in Instructions.CACHE:
            return Instructions.CACHE[kernel.registers.pc]
        opcode = inst & 0b1111111
        match opcode:
            case 0b0010011:
                inst = IType(inst).decode()
                match inst.funct3:
                    case 0:
                        return Instruction(inst, Instructions.addi)
                    case 0b001:
                        return Instruction(inst, Instructions.slli)
                    case 0b010:
                        return Instruction(inst, Instructions.slti)
                    case 0b011:
                        return Instruction(inst, Instructions.sltiu)
                    case 0b100:
                        return Instruction(inst, Instructions.xori)
                    case 0b110:
                        return Instruction(inst, Instructions.ori)
                    case 0b111:
                        return Instruction(inst, Instructions.andi)

                    case 0b101:
                        if inst.imm >> 10 == 1:
                            return Instruction(inst, Instructions.srai)
                        elif inst.imm >> 10 == 0:
                            return Instruction(inst, Instructions.srli)
                        else:
                            raise Exception("not gud")
                    case _:
                        raise Exception(f"Wrong func3 ({bin(inst.funct3)}) for opcode: ", bin(opcode))

            case 0b0110011:
                inst = RType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        if inst.funct7 >> 5 == 0:
                            return Instruction(inst, Instructions.add)
                        elif inst.funct7 >> 5 == 1:
                            return Instruction(inst, Instructions.sub)
                        else:
                            raise Exception("Wrong func7")

                    case 0b001:
                        return Instruction(inst, Instructions.sll)

                    case 0b010:
                        return Instruction(inst, Instructions.slt)

                    case 0b011:
                        return Instruction(inst, Instructions.sltu)

                    case 0b100:
                        return Instruction(inst, Instructions.xor)

                    case 0b101:
                        if inst.funct7 >> 5 == 0:
                            return Instruction(inst, Instructions.srl)
                        elif inst.funct7 >> 5 == 1:
                            return Instruction(inst, Instructions.sra)
                        else:
                            raise Exception("Wrong func7")

                    case 0b110:
                        return Instruction(inst, Instructions._or)

                    case 0b111:
                        return Instruction(inst, Instructions._and)

            case 0b0110111:
                return Instruction(UType(inst).decode(), Instructions.lui)

            case 0b0010111:
                return Instruction(UType(inst).decode(), Instructions.auipc)
                # not tested

            case 0b1101111:
                return Instruction(JType(inst).decode(), Instructions.jal)

            case 0b0000011:
                # Load instructions
                inst = IType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        return Instruction(inst, Instructions.lb)
                    case 0b001:
                        return Instruction(inst, Instructions.lh)
                    case 0b100:
                        return Instruction(inst, Instructions.lbu)
                    case 0b101:
                        return Instruction(inst, Instructions.lhu)
                    case 0b010:
                        return Instruction(inst, Instructions.lw)
                    case 0b110:
                        return Instruction(inst, Instructions.lwu)
                    case 0b011:
                        return Instruction(inst, Instructions.ld)

            case 0b0100011:
                # Store instructions
                inst = SType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        return Instruction(inst, Instructions.sb)
                    case 0b001:
                        return Instruction(inst, Instructions.sh)
                    case 0b010:
                        return Instruction(inst, Instructions.sw)
                    case 0b011:
                        return Instruction(inst, Instructions.sd)

            case 0b1100011:
                # Branch instructions
                inst = BType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        return Instruction(inst, Instructions.beq)
                    case 0b001:
                        return Instruction(inst, Instructions.bne)
                    case 0b100:
                        return Instruction(inst, Instructions.blt)
                    case 0b101:
                        return Instruction(inst, Instructions.bge)
                    case 0b110:
                        return Instruction(inst, Instructions.bltu)
                    case 0b111:
                        return Instruction(inst, Instructions.bgeu)

            case 0b0111011:
                # W instructions
                inst = RType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        if inst.funct7 >> 5 == 0:
                            return Instruction(inst, Instructions.addw)
                        elif inst.funct7 >> 5 == 1:
                            return Instruction(inst, Instructions.subw)
                        else:
                            raise Exception("Wrong func7")

                    case 0b001:
                        return Instruction(inst, Instructions.sllw)

                    case 0b101:
                        if inst.funct7 >> 5 == 0:
                            return Instruction(inst, Instructions.srlw)
                        elif inst.funct7 >> 5 == 1:
                            return Instruction(inst, Instructions.sraw)
                        else:
                            raise Exception("Wrong func7")
            case 0b0011011:
                # W immediate instructions
                inst = IType(inst).decode()
                match inst.funct3:
                    case 0b000:
                        return Instruction(inst, Instructions.addiw)
                    case 0b001:
                        return Instruction(inst, Instructions.slliw)
                    case 0b101:
                        if inst.imm >> 10 == 0:
                            return Instruction(inst, Instructions.srliw)
                        elif inst.imm >> 10 == 1:
                            # TODO test
                            return Instruction(inst, Instructions.sraiw)
                        else:
                            raise Exception("Wrong immediate end end encoding thing")
            case 0b1100111:
                return Instruction(IType(inst).decode(), Instructions.jalr)

            case 0b1110011:
                # ECall
                return Instruction(IType(inst).decode(), Instructions.ecall)

            case 0b1111011:
                # EBreak
                return Instruction(IType(inst).decode(), Instructions.ebreak)

            case 0b0001111:
                # Fence
                return Instruction(IType(inst).decode(), Instructions.fence)

            case _:
                a = Instructions_A.decode(inst)
                if a:
                    return a
                kernel.log("Instruction: ", hex(inst), priority=3)
                kernel.exception(f"Unknown opcode: {bin(opcode)}")

        kernel.exception("Unknown opcode 2")


"""
if __name__ == '__main__':

    #   ff650493  | 111111110110 01010 000 010010 010011        	add	s1,a0,10
    # print(bin(0xff650493))
    instruction_list = [0x00b50593, 0x00359593, 0x4d25f593, 0x3944d537, 0x4d350513, 0x4dd57593, 0x00a58633, 0x40c586b3,
                        0x01965613, 0x00c69733, 0x00e6a7b3, 0x00072833, 0x00e038b3, 0x00e74733, 0x00060733, 0xfec70713,
                        0x00e6d4b3, 0x40e6d433, 0x40b55393, 0x00d686b3]
    kernel.registers = Registers()
    instructions = []
    for i in instruction_list:
        instructions.append(Instructions.match(i))

    for i in instructions:
        i.do(kernel.registers)
    print(kernel.registers)

    # Test oveflow -> sollte passen
    addi x11, x10, 11
    slli x11, x11, 3
    andi x11, x11, 1234
    lui x10, 234573
    addi x10, x10, 1235
    andi x11, x10, 1245
    add x12, x11, x10
    sub x13, x11, x12
    srli x12, x12, 25
    sll x14, x13, x12
    slt x15, x13, x14
    slt x16, x14, x0
    sltu x17, x0, x14
    xor x14, x14, x14
    add x14, x12, x0"""
