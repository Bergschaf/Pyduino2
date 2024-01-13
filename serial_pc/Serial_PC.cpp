//
// Created by bergschaf on 1/13/24.
//
# include <stdio.h>
# include <unistd.h>
#include <CppLinuxSerial/SerialPort.hpp>

using namespace mn::CppLinuxSerial;
using namespace std;

int decode_request(vector<uint8_t> bytes) {
    if (bytes.size() <= 2) {
        return 0;
    }
    uint8_t first_byte = bytes[0];
    switch (first_byte) {
        case 0x01: {
            // print request
            uint8_t size_byte = bytes[1];
            // print the bytes colored
            printf("\033[1;31m");
            for (int i = 0; i < size_byte; ++i) {
                printf("%c", bytes[i + 2]);
            }
            printf("\033[0m\n");
            printf("Printed %d bytes\n", size_byte);
            return size_byte + 2;
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
            printf("Read from serial port: %x | %ld\n", bytes[0], bytes.size());
            while (bytes.size() > 0) {
                printf("Read from serial port: %x | %ld\n", bytes[0], bytes.size());
                uint to_delete = decode_request(bytes);
                printf("to_delete: %d\n", to_delete);
                if(to_delete == 0){
                    break;
                }
                // delete to-delete bytes
                if (to_delete > bytes.size()) {
                    break;
                }
                bytes = vector<uint8_t>(bytes.begin() + to_delete, bytes.end());
            }
        }
    }
}

int main() {
    SerialPort serialPort("/dev/ttyACM0", BaudRate::B_9600, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    serialPort.SetTimeout(0);

    serialPort.Open();
    // delay 1s
    usleep(3000000);
    printf("Write to serial port\n");
    listener(serialPort);
    serialPort.Close();
    return 0;
}
