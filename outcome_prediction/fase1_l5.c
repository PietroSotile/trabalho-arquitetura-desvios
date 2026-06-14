#include <stdio.h>

int main() {
    volatile int a = 0;
    long unsigned liter = 10000000;
    for (long unsigned i = 0; i < liter; i++) {
        if ((i % 5) == 0) a = 0;
    }
    return 0;
}
