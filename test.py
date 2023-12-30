XLEN = 64


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

print(bin(int_to_bin(se(int_from_bin(0b111111111110110110100,21)))))
print(bin(sign_extend(0b111111111110110110100,21)))
print(bin(0xffffffff))