//
// Created by bergschaf on 1/13/24.
//
# include <stdio.h>
# include <unistd.h>
#include <CppLinuxSerial/SerialPort.hpp>

using namespace mn::CppLinuxSerial;
using namespace std;

#include <thread>


#define ELF_FILE_START_BYTE 0x10
#define ELF_FILE_ACK_BYTE 0x20
#define MEMORY_SIZE 100000000

int to_send = 0;
bool elf_start = false;
uint8_t *memory = (uint8_t *) malloc(MEMORY_SIZE);


void transmit_elf_file(string filename, SerialPort &serialPort) {
    FILE *file = fopen(filename.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *data = (uint8_t *) malloc(file_size);
    fread(data, 1, file_size, file);
    fclose(file);

    // send elf file start byte
    serialPort.WriteBinary({ELF_FILE_START_BYTE});
    // wait for ack byte TODO make better
    while (!elf_start) {}

    printf("Sending header, file size: %ld\n", file_size);

    while (1) {
        if (to_send > 0) {
            //printf("Sending %d bytes\n", to_send);
            serialPort.WriteBinary(vector<uint8_t>(data, data + to_send));
            data += to_send;
            to_send = 0;
        }
    }
}

int decode_request(vector<uint8_t> bytes, SerialPort &serialPort) {
    uint8_t first_byte = bytes[0];
    switch (first_byte) {
        case 0x01: {
            // print request
            uint8_t size_byte = bytes[1];
            // print the bytes colored
            if (size_byte > bytes.size()) {
                return 0;
            }
            printf("\033[1;31m");
            for (int i = 0; i < size_byte; ++i) {
                printf("%c", bytes[i + 2]);
            }
            printf("\033[0m\n");
            //printf("Printed %d bytes\n", size_byte);
            return size_byte + 2;
        }
        case 0x02: {
            // elf requst
            //printf("Length: %d\n", bytes[1]);
            uint8_t size_byte = bytes[1];
            to_send = size_byte;
            /*
            if(size_byte < 64){
                // print the content of the memory
                for (int i = 0; i < 0x12000; ++i) {
                    printf("%x | %x\n", i, memory[i]);
                }
            }*/

            return 2;
        }

        case 0x03: {
            // write to memory
            uint64_t address = 0;
            for (int i = 0; i < 8; ++i) {
                address |= bytes[i + 1] << (i * 8);
            }
            uint16_t length = 0;
            length |= bytes[9] << 8;
            length |= bytes[10];

            if (length > bytes.size() - 11) {
                return 0;
            }
            /*printf("Store Address: %lx\n", address);
            printf("Store Length: %d\n", length);
            if (length == 4) {
                uint64_t res = 0;
                for (int i = 0; i < 4; ++i) {
                    res |= bytes[i + 11] << (i * 8);
                }
                printf("Store Value: %lx\n", res);
            } else if (length == 8) {
                uint64_t res = 0;
                for (int i = 0; i < 8; ++i) {
                    res |= bytes[i + 11] << (i * 8);
                }
                printf("Store Value: %lx\n", res);
            } else {
                printf("Store Value: ");
                for (int i = 0; i < length; ++i) {
                    printf("%x ", bytes[i + 11]);
                }
                printf("\n");
            }*/

            for (int i = 0; i < length; ++i) {
                memory[address + i] = bytes[i + 11];
            }
            return length + 11;
        }
        case 0x4: {
            // read from memory
            uint64_t address = 0;
            for (int i = 0; i < 8; ++i) {
                address |= bytes[i + 1] << (i * 8);
            }
            uint16_t length = 0;
            length |= bytes[9] << 8;
            length |= bytes[10];

            if (bytes.size() < 11) {
                return 0;
            }


            //printf("Address: %lx | Length: %d\n", address, length);
            //printf("Length: %d\n", length);
            // print all bytes
            //for (int i = 0; i < bytes.size(); ++i) {
            //    printf("%x ", bytes[i]);
            //}
            bytes = vector<uint8_t>(length);
            for (int i = 0; i < length; ++i) {
                bytes[i] = memory[address + i];
            }
            serialPort.WriteBinary({0x05});
            serialPort.WriteBinary(bytes);
            // print all bytes sent as a 32bit integer
            /*
            if (length == 4) {
                uint64_t res = 0;
                for (int i = 0; i < 4; ++i) {
                    res |= bytes[i] << (i * 8);
                }
                printf("Sent: %lx\n", res);
            } else if (length == 8) {
                uint64_t res = 0;
                for (int i = 0; i < 8; ++i) {
                    res |= bytes[i] << (i * 8);
                }
                printf("Sent: %lx\n", res);
            }*/
            return 11;

        }

        case ELF_FILE_ACK_BYTE: {
            // elf file start byte
            elf_start = true;
        }

        default: {
            printf("Unknown request: %d\n", first_byte);
            return 1;
        }
    }
}

[[noreturn]] void listener(SerialPort &serialPort) {
    vector<uint8_t> bytes;
    while (true) {
        if (serialPort.Available() > 0) {
            serialPort.ReadBinary(bytes);
            //printf("Read from serial port: %x | %ld\n", bytes[0], bytes.size());
            while (bytes.size() > 0) {
                //printf("Read from serial port: %x | %ld\n", bytes[0], bytes.size());
                uint to_delete = decode_request(bytes, serialPort);
                //printf("to_delete: %d\n", to_delete);
                if (to_delete == 0) {
                    break;
                }
                // delete to-delete bytes
                if (to_delete > bytes.size()) {
                    bytes = vector<uint8_t>();
                    break;
                }
                bytes = vector<uint8_t>(bytes.begin() + to_delete, bytes.end());
            }
        }
    }
}

#define HANDSHAKE_RECEIVE_BYTE 0x42
#define HANDSHAKE_SEND_BYTE 0x23

void first_handshake(SerialPort &serialPort) {
    // wait for handshake byte
    while (true) {
        if (serialPort.Available() > 0) {
            vector<uint8_t> bytes;
            serialPort.ReadBinary(bytes);
            printf("Read from serial port: %x | %ld\n", bytes[0], bytes.size());
            if (bytes[0] == HANDSHAKE_RECEIVE_BYTE) {
                // send handshake byte
                vector<uint8_t> handshake_send_byte = {HANDSHAKE_SEND_BYTE};
                for (int i = 0; i < 1000; ++i) {
                    serialPort.WriteBinary(handshake_send_byte);
                }
                return;
            }
            bytes = vector<uint8_t>();
        }
    }
}

int main() {
    SerialPort serialPort("/dev/ttyACM0", BaudRate::B_460800, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    serialPort.SetTimeout(0);

    serialPort.Open();
    // sleep for 3 s
    // intialize the memory with 0
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        memory[i] = 0;
    }

    sleep(3);
    first_handshake(serialPort);
    // print colored
    printf("\033[1;31m");
    printf("Opened serial port\n");
    printf("\033[0m\n");
    // delay 1s

    // start thread that sends the elf file
    thread t(transmit_elf_file, "/home/bergschaf/PycharmProjects/Pyduino2/serial_pc/test", ref(serialPort));

    listener(serialPort);
    serialPort.Close();
    return 0;
}
