

class RegistersExpectedState:
    def __init__(self, **kwargs):
        self.expected = dict(**kwargs)
        self.registers = [self.expected[f"x{i}"] if f"x{i}" in self.expected else None for i in range(32)]
        self.pc = self.expected["pc"] if "pc" in self.expected else None

    def compare(self, registers):
        for i in range(32):
            if self.registers[i] is not None:
                assert self.registers[i] == registers[i]
        if self.pc is not None:
            assert self.pc == registers.pc
        return True
