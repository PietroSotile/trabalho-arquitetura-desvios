#include <stdio.h>


int main() {
    volatile int x = 5;
    if (x < 10) x = 0;
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    return 0;
}

// RESULTADO:
//     BRANCH SE TORNA:
    //     movl  -4(%rbp), %eax    ; carrega x
    //     cmpl  $9, %eax          ; compara
    //     jg    .L2               ; branch em si
    //     movl  $0, -4(%rbp)      ; x = 0

// Tamanho em bytes:
    // 140001464:  8b 45 fc    mov  -0x4(%rbp),%eax    -> 3 bytes
    // 140001467:  83 f8 09    cmp  $0x9,%eax           -> 3 bytes
    // 14000146a:  7f 07       jg   .L2                 -> 2 bytes
    // 14000146c:  c7 45 fc 00 movl $0x0,-0x4(%rbp)    -> 7 bytes