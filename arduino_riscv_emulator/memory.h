//
// Created by bergschaf on 1/13/24.
//

#ifndef ATMEGA_SERIAL_MEMORY_H
#define ATMEGA_SERIAL_MEMORY_H
#include <stdint.h>

void memory_init(void);

void memory_write(uint64_t address, uint8_t *DataBytes, int length);

void memory_load(uint64_t address, uint8_t *DataBytes, int length);

#endif //ATMEGA_SERIAL_MEMORY_H
