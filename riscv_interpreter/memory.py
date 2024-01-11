import numpy as np

class Memory:
    def __init__(self, size):
        self.start_adress = 0 # TODO nicht so gut
        self.size = size
        self.last_address = size
        self.memory = np.zeros(size, dtype=np.uint8)
        self.reservations = set()
        self.max_store_adress = 0

    def mmap_anonymous(self, size):
        address = self.last_address  - size - 1 + self.start_adress
        self.last_address = address
        print(address)
        return address


    def loads(self, address):
        # terminates at 0
        string = ""
        while True:
            char = self.load_byte(address)
            if char == 0:
                break
            if len(string) > 1000:
                raise Exception("String too long")
            string += chr(char)
            address += 1
        return string

    def puts(self, address, string):
        for i, char in enumerate(string):
            self.store_byte(address + i, ord(char))
        self.store_byte(address + len(string), 0)

    def store_bytes(self, address, bytes):
        #print("store: ", address, bytes)
        self.max_store_adress = max(self.max_store_adress, address + len(bytes) - self.start_adress)
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
        #print("load : ", address, size, self.memory[address - self.start_adress: address - self.start_adress + size])
        return self.memory[address - self.start_adress: address - self.start_adress + size]

    def load_word(self, address):
        return int.from_bytes(self.load_bytes(address, 4), byteorder="little")

    def load_doubleword(self, address):
        #print("load dw:", address, hex(int.from_bytes(self.load_bytes(address, 8), byteorder="little")))
        return int.from_bytes(self.load_bytes(address, 8), byteorder="little")

    def load_halfword(self, address):
        return int.from_bytes(self.load_bytes(address, 2), byteorder="little")

    def load_byte(self, address):
        return int.from_bytes(self.load_bytes(address, 1), byteorder="little")


    def __repr__(self):
        return f"Memory(size={self.size})"
