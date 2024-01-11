//
// Created by bergschaf on 1/11/24.
//
#include <stdio.h>
#include <stdint.h>
int main(){
    uint8_t a = 0x80;
    printf("%lx\n", (int64_t)a << 56);

}