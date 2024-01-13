//
// Created by bergschaf on 1/13/24.
//

#ifndef ATMEGA_SERIAL_SERIAL_H
#define ATMEGA_SERIAL_SERIAL_H

//#define F_CPU 16000000UL // Defining the CPU Frequency

#include <avr/io.h>      // Contains all the I/O Register Macros

#define USART_BAUDRATE 460800 	 // Desired Baud Rate
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

#define HANDSHAKE_BYTE 0x42
#define HANDSHAKE_ACK_BYTE 0x23

#define ELF_FILE_START_BYTE 0x10
#define ELF_FILE_ACK_BYTE 0x20

#define CHUNK_SIZE 64

void USART_Init(void);

uint8_t USART_ReceivePolling(void);

void USART_ReceiveBytes(uint8_t *DataBytes, int length);

void USART_TransmitPolling(uint8_t *DataBytes, int length);

void serial_printf(const char *format, ...);

int USART_WaitForByte(uint8_t Byte, int timeout_cycles);

void USART_WaitForByteInfinite(uint8_t Byte);

void first_handshake(void);

#endif //ATMEGA_SERIAL_SERIAL_H
