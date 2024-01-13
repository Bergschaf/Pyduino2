//
// Created by bergschaf on 1/13/24.
//

#include "memory.h"
#include "serial.h"

void memory_init(void) {

}

void memory_write(uint64_t address, uint8_t *DataBytes, int length) {
    const uint8_t instruction_byte = 0x03;
    USART_TransmitPolling(&instruction_byte, 1);

    uint8_t address_bytes[8];
    for (int i = 0; i < 8; ++i) {
        address_bytes[i] = (uint8_t) (address >> (i * 8));
    }
    USART_TransmitPolling(address_bytes, 8);

    uint8_t length_bytes[2];
    length_bytes[0] = (uint8_t) (length >> 8);
    length_bytes[1] = (uint8_t) (length);
    USART_TransmitPolling(length_bytes, 2);

    USART_TransmitPolling(DataBytes, length);
}

void memory_load(uint64_t address, uint8_t *DataBytes, int length) {
    const uint8_t instruction_byte = 0x04;
    USART_TransmitPolling(&instruction_byte, 1);

    uint8_t address_bytes[8];
    for (int i = 0; i < 8; ++i) {
        address_bytes[i] = (uint8_t) (address >> (i * 8));
    }
    USART_TransmitPolling(address_bytes, 8);

    uint8_t length_bytes[2];
    length_bytes[0] = (uint8_t) (length >> 8);
    length_bytes[1] = (uint8_t) (length);
    USART_TransmitPolling(length_bytes, 2);

    serial_printf("Waiting for byte\n");
    USART_WaitForByteInfinite(0x05);
    serial_printf("Received byte\n");

    for (int i = 0; i < length; ++i) {
        DataBytes[i] = USART_ReceivePolling();
    }
}
