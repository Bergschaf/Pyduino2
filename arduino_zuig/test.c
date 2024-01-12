#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#define MS_DELAY 100

int main (void) {
    /*Set to one the fifth bit of DDRB to one
    **Set digital pin 13 to output mode */
    DDRB |= _BV(DDB5);
    // Set frame format: 8data, 1stop bit
    uart_init();
    while(1) {
        // print hello world to serial with baudrate 9600
        printf("Hello world\n");



        /*Set to one the fifth bit of PORTB to one
        **Set to HIGH the pin 13 */
        PORTB |= _BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(MS_DELAY);

        /*Set to zero the fifth bit of PORTB
        **Set to LOW the pin 13 */
        PORTB &= ~_BV(PORTB5);

        /*Wait 3000 ms */
        _delay_ms(MS_DELAY);
    }
}
