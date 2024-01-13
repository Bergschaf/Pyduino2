//
// Created by bergschaf on 1/13/24.
//

#include "serial.h"
/*
* usart.c
*
* Created : 15-08-2020 08:34:15 PM
* Author  : Arnab Kumar Das
* Website : www.ArnabKumarDas.com
*/


void USART_Init(void) {
    // Set Baud Rate
    UBRR0H = BAUD_PRESCALER >> 8;
    UBRR0L = BAUD_PRESCALER;

    // Set Frame Format
    UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

    // Enable Receiver and Transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

uint8_t USART_ReceivePolling(void) {
    uint8_t DataByte;
    while ((UCSR0A & (1 << RXC0)) == 0) {}; // Do nothing until data have been received
    DataByte = UDR0;
    return DataByte;
}


void USART_ReceiveBytes(uint8_t *DataBytes, int length) {
    // send the instruction byte, length must not be greater than 255
    const uint8_t instruction_byte = 0x02;
    USART_TransmitPolling(&instruction_byte,1);
    USART_TransmitPolling(&length,1);
    for (int i = 0; i < length; ++i) {
        DataBytes[i] = USART_ReceivePolling();
    }
}


void USART_TransmitPolling(uint8_t *DataBytes, int length) {
    for (int i = 0; i < length; ++i) {
        while ((UCSR0A & (1 << UDRE0)) == 0) {}; // Do nothing until UDR is ready
        UDR0 = DataBytes[i];
    }
}


void USART_WaitForByte(uint8_t Byte, int timeout_cycles) {
    for (int i = 0; i < timeout_cycles; ++i) {
        if ((UCSR0A & (1 << RXC0)) == 1){
            if (UDR0 == Byte){
                return;
            }
        }
    }
}

void USART_WaitForByteInfinite(uint8_t Byte) {
    while (1){
        if ((UCSR0A & (1 << RXC0)) == 1){
            if (UDR0 == Byte){
                return;
            }
        }
    }
}

void do_serial_print(char* string, int length) {
    // transmit the instruction byte
    const uint8_t instruction_byte = 0x01;
    USART_TransmitPolling(&instruction_byte, 1);
    int offset = 0;
    while(length >= 0){
        uint8_t length_byte = length;
        if (length > 255){
            length_byte = 255;
        }
        USART_TransmitPolling(&length_byte, 1);
        USART_TransmitPolling(string + offset, length_byte);
        length -= 255;
        offset += 255;
    }
}

void first_handshake(void){
    const uint8_t handshake_byte = HANDSHAKE_BYTE;
    while (1){
        USART_TransmitPolling(&handshake_byte, 1);
        USART_WaitForByte(HANDSHAKE_ACK_BYTE, 1000);
    }
}