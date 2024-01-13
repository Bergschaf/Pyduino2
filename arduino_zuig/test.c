/*
* usart.c
*
* Created : 15-08-2020 08:34:15 PM
* Author  : Arnab Kumar Das
* Website : www.ArnabKumarDas.com
*/

#define F_CPU 16000000UL // Defining the CPU Frequency

#include <avr/io.h>      // Contains all the I/O Register Macros
#include <util/delay.h>  // Generates a Blocking Delay

#define USART_BAUDRATE 9600 // Desired Baud Rate
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

void USART_Init() {
    // Set Baud Rate
    UBRR0H = BAUD_PRESCALER >> 8;
    UBRR0L = BAUD_PRESCALER;

    // Set Frame Format
    UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

    // Enable Receiver and Transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

uint8_t USART_ReceivePolling() {
    uint8_t DataByte;
    while ((UCSR0A & (1 << RXC0)) == 0) {}; // Do nothing until data have been received
    DataByte = UDR0;
    return DataByte;
}

void USART_TransmitPolling(uint8_t *DataBytes, int length) {
    for (int i = 0; i < length; ++i) {
        while ((UCSR0A & (1 << UDRE0)) == 0) {}; // Do nothing until UDR is ready
        UDR0 = DataBytes[i];
    }
}

void do_print(char* string, int length) {
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


int main() {
    USART_Init();
    char *LocalData = "Hello World!\n";
    while (1) {
        do_print(LocalData, 13);
        _delay_ms(1000);
    }
    return 0;
}