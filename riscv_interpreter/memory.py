import numpy as np

class Memory:
    def __init__(self, size):
        self.start_adress = 0
        self.size = size
        self.memory = np.zeros(size, dtype=np.uint8)

    def store_bytes(self, address, bytes):
        for i, byte in enumerate(bytes):
            self.memory[address + i - self.start_adress] = byte

    def store_doubleword(self, address, doubleword):
        self.store_bytes(address, doubleword.to_bytes(8, byteorder="little"))
    def store_word(self, address, word):
        self.store_bytes(address, word.to_bytes(4, byteorder="little"))

    def store_halfword(self, address, halfword):
        self.store_bytes(address, halfword.to_bytes(2, byteorder="little"))

    def store_byte(self, address, byte):
        self.store_bytes(address, byte.to_bytes(1, byteorder="little"))

    def load_bytes(self, address, size):
        return self.memory[address - self.start_adress: address - self.start_adress + size]

    def load_word(self, address):
        return int.from_bytes(self.load_bytes(address, 4), byteorder="little")

    def load_doubleword(self, address):
        return int.from_bytes(self.load_bytes(address, 8), byteorder="little")

    def load_halfword(self, address):
        return int.from_bytes(self.load_bytes(address, 2), byteorder="little")

    def load_byte(self, address):
        return int.from_bytes(self.load_bytes(address, 1), byteorder="little")


    def __repr__(self):
        return f"Memory(size={self.size})"
