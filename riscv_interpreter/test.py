from expected import RegistersExpectedState
from main import run_test_file
from interpreter import int_to_bin, sign_extend, int_from_bin


def compile_asm_instruction_list(asm):
    # list of strings of asm instructions
    # returns file path to executable
    asm_str = """.global _start
.section .text
_start:""" + "\n".join(asm) + "\nebreak\n"
    abs_tmp_dir = "/home/bergschaf/PycharmProjects/Pyduino2/riscv_interpreter/tmp/"
    with open(abs_tmp_dir + "tmp.s", "w") as f:
        f.write(asm_str)
    import subprocess
    subprocess.run(["pwd"])
    subprocess.run(["riscv64-elf-as", abs_tmp_dir + "tmp.s", "-o", abs_tmp_dir + "tmp.o"])
    subprocess.run(["riscv64-elf-ld", abs_tmp_dir + "tmp.o", "-o", abs_tmp_dir + "tmp.out"])
    return abs_tmp_dir + "tmp.out"


def run_asm_instruction_list(asm):
    # list of strings of asm instructions
    # returns RegistersExpectedState
    file = compile_asm_instruction_list(asm)
    return run_test_file(file)


def run_test(instruction_list, expected_dict):
    # instruction_list: list of strings of asm instructions
    # expected_dict: dict of register names and expected values
    registers, memory = run_asm_instruction_list(instruction_list)
    expected_dict = {x: expected_dict[x] for x in expected_dict}  # only for riscv64
    expected = RegistersExpectedState(**expected_dict)
    expected.compare(registers)


def test_addi():
    run_test(["addi x1, x0, 1234"], {"x1": 1234})
    run_test(["addi x1, x0, -1234"], {"x1": -1234})
    run_test(["addi x1, x0, 0"], {"x1": 0})
    run_test(["addi x0, x1, 1234"], {"x0": 0})
    run_test(["addi x1, x0, 1000", "addi x2, x1, 234"], {"x2": 1234})


def test_lui():
    run_test(["lui x1, 1234"], {"x1": 1234 << 12})
    run_test(["lui x1, 0"], {"x1": 0})
    run_test(["lui x0, 1234"], {"x0": 0})


def test_registers_zero_at_beginning():
    run_test(["addi x0, x0, 1234"], {f"x{i}": 0 for i in range(32)})


def test_add():
    run_test(["add x1, x0, x0"], {"x1": 0})
    run_test(["add x1, x0, x0", "add x2, x1, x0"], {"x2": 0})
    run_test(["addi x1, x0, 1234", "addi x2, x0, 1678", "add x3, x1, x2"], {"x3": 2912})

    # test_overflow# TDOO


def test_store_load_word():
    # test zero adress
    run_test(["lui x1, 123455", "addi x1, x1, 1234", "sw x1, 0(x0)", "lw x2, 0(x0)"], {"x2": 1234 + 123455 * 2 ** 12})
    run_test(["addi x1, x1, 1234", "sw x1, 0(x0)", "addi x1, x1, 1", "lw x2, 0(x0)"], {"x2": 1234})

    # test negative address
    run_test(["addi x1, x1, 1235", "addi x2, x0, -10", "sw x1, 0(x2)", "lw x3, 0(x2)"], {"x3": 1235})

    # test positive address
    run_test(["addi x1, x1, 1235", "addi x2, x0, 100", "sw x1, 0(x2)", "lw x3, 0(x2)"], {"x3": 1235})


def test_load_store_byte():
    # test zero adress
    run_test(["lui x1, 123455", "addi x1, x1, 1234", "sb x1, 0(x0)", "lb x2, 0(x0)"],
             {"x2": int_from_bin(sign_extend((1234 + 123455 * 2 ** 12) & 0xFF, 8))})
    run_test(["addi x1, x1, 1234", "sb x1, 0(x0)", "addi x1, x1, 1", "lb x2, 0(x0)"],
             {"x2": int_from_bin(sign_extend(1234 & 0xFF, 8))})

    # test negative address
    run_test(["addi x1, x1, 1235", "addi x2, x0, -10", "sb x1, 0(x2)", "lb x3, 0(x2)"],
             {"x3": int_from_bin(sign_extend(1235 & 0xFF, 8))})

    # test positive address
    run_test(["addi x1, x1, 1235", "addi x2, x0, 100", "sb x1, 0(x2)", "lb x3, 0(x2)"],
             {"x3": int_from_bin(sign_extend(1235 & 0xFF, 8))})

def test_load_store_halfword():
    # test zero adress
    run_test(["lui x1, 123455", "addi x1, x1, 1234", "sh x1, 0(x0)", "lh x2, 0(x0)"],
             {"x2": int_from_bin(sign_extend((1234 + 123455 * 2 ** 12) & 0xFFFF, 16))})
    run_test(["addi x1, x1, 1234", "sh x1, 0(x0)", "addi x1, x1, 1", "lh x2, 0(x0)"],
             {"x2": int_from_bin(sign_extend(1234 & 0xFFFF, 16))})

    # test negative address
    run_test(["addi x1, x1, 1235", "addi x2, x0, -10", "sh x1, 0(x2)", "lh x3, 0(x2)"],
             {"x3": int_from_bin(sign_extend(1235 & 0xFFFF, 16))})

    # test positive address
    run_test(["addi x1, x1, 1235", "addi x2, x0, 100", "sh x1, 0(x2)", "lh x3, 0(x2)"],
             {"x3": int_from_bin(sign_extend(1235 & 0xFFFF, 16))})

def test_load_store_doubleword():
    run_test(["lui x1, 123455", "addi x1, x1, 1234", "sd x1, -100(x0)", "ld x2, -100(x0)"],
             {"x2": 1234 + 123455 * 2 ** 12})
    run_test(["addi x1, x1, 1234", "sd x1, 0(x0)", "addi x1, x1, 1", "ld x2, 0(x0)"],
             {"x2": 1234})

    # test negative address
    run_test(["addi x1, x1, 1235", "addi x2, x0, -10", "sd x1, 0(x2)", "ld x3, 0(x2)"],
             {"x3": 1235})

    # test positive address
    run_test(["addi x1, x1, 1235", "addi x2, x0, 100", "sd x1, 0(x2)", "ld x3, 0(x2)"],
             {"x3": 1235})

def test_slli():
    run_test(["addi x1, x0, 1234", "slli x2, x1, 2"], {"x2": 1234 << 2})
    run_test(["addi x1, x0, 1234", "slli x2, x1, 0"], {"x2": 1234})
    run_test(["addi x1, x0, 1234", "slli x2, x1, 32"], {"x2": 1234 << 32})
    run_test(["addi x1, x0, 1234", "slli x2, x1, 31"], {"x2": 1234 << 31})
    run_test(["addi x1, x0, 1", "slli x2, x1, 63"], {"x2": int_from_bin(1 << 63)})

def test_srli():
    run_test(["addi x1, x0, 1234", "srli x2, x1, 2"], {"x2": 1234 >> 2})
    run_test(["addi x1, x0, 1234", "srli x2, x1, 0"], {"x2": 1234})
    run_test(["addi x1, x0, 1234","slli x2, x1, 32", "srli x2, x2, 32"], {"x2": 1234})
    run_test(["addi x1, x0, 1234", "srli x2, x1, 31"], {"x2": 0})

def test_srai():
    run_test(["addi x1, x0, 1234", "srai x2, x1, 2"], {"x2": 1234 >> 2})
    run_test(["addi x1, x0, 1234", "srai x2, x1, 0"], {"x2": 1234})
    run_test(["addi x1, x0, 0b1111", "srai x2, x1, 2"], {"x2": 0b11})
    run_test(["addi x1, x0, 1234", "srai x2, x1, 31"], {"x2": 1234 >> 31})

def test_andi():
    run_test(["addi x1, x0, 1234", "andi x2, x1, 0"], {"x2": 0})
    run_test(["addi x1, x0, 0b1110", "andi x2, x1, 0b1111"], {"x2": 0b1110})
    run_test(["addi x1, x0, 0b11111000011", "andi x2, x1, 0b1111111"], {"x2": 0b1000011})

def test_ori():
    run_test(["addi x1, x0, 1234", "ori x2, x1, 0"], {"x2": 1234})
    run_test(["addi x1, x0, 0b1110", "ori x2, x1, 0b1111"], {"x2": 0b1111})
    run_test(["addi x1, x0, 0b11111000011", "ori x2, x1, 0b1111111"], {"x2": 0b11111111111})



if __name__ == '__main__':
    test_addi()
    test_add()
    test_registers_zero_at_beginning()
