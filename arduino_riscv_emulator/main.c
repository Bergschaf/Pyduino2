/*
* usart.c
*
* Created : 15-08-2020 08:34:15 PM
* Author  : Arnab Kumar Das
* Website : www.ArnabKumarDas.com
*/

#include "cpu.h"
#include "serial.h"

int main(void) {
    USART_Init();
    // wait for transmission
    first_handshake();

    Cpu *cpu = malloc(sizeof(Cpu));
    load_elf_executable("sl", cpu);
    run(cpu);
    return 111;
}
