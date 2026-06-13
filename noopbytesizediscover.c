#include <stdio.h>

//Em x86-64 o nop deve ocupar 1 byte, sendo uma instrução isolada
int main() {
    volatile int x = 5;
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    return 0;
}