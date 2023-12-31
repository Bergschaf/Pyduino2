from interpreter import int_to_bin, int_from_bin

class RegistersExpectedState:
    def __init__(self, **kwargs):
        self.expected = dict(**kwargs)
        self.registers = [self.expected[f"x{i}"] if f"x{i}" in self.expected else None for i in range(32)]
        self.pc = self.expected["pc"] if "pc" in self.expected else None

    def compare(self, registers):
        for i in range(32):
            if self.registers[i] is not None:
                print(f"Expected: {self.registers[i]}")
                print(f"Actual:   {int_from_bin(registers[i])}")
                assert self.registers[i] == int_from_bin(registers[i])
        if self.pc is not None:
            print(self.pc, registers.pc)
            assert self.pc == registers.pc
        return True
