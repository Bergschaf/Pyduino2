/*
* usart.c
*
* Created : 15-08-2020 08:34:15 PM
* Author  : Arnab Kumar Das
* Website : www.ArnabKumarDas.com
*/

#include "cpu.h"
#include "serial.h"
#include <avr/delay.h>
#include <string.h>
#include "memory.h"

int main(void) {
    USART_Init();
    // wait for transmission
    first_handshake();
    Cpu *cpu = malloc(sizeof(Cpu));
    load_elf_executable("uuu", cpu);
    while (1) {
        // loopback
        serial_printf("Hello from the ATmega328p\n");

        //do_serial_print("Hello from the ATmega328p\n", 27);
        //_delay_ms(1000);
        //Cpu *cpu = malloc(sizeof(Cpu));
        //load_elf_executable("sl", cpu);
        run(cpu);
        serial_printf("Done\n");
        // delay
        _delay_ms(1000000);
        //return 111;
    }
}
