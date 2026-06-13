#include <stdio.h>
#include <stdlib.h>

void gerar_benchmark(int n_branches, int distancia, int n_iter, const char *filename) {
    int n_nops = distancia - 15;
    if (n_nops < 0) {
        printf("Distancia muito pequena, minimo e 15\n");
        return;
    }

    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Erro ao abrir arquivo\n");
        return;
    }

    fprintf(f, "#include <stdio.h>\n\n");
    fprintf(f, "int main() {\n");
    fprintf(f, "    volatile int x = 5;\n");
    fprintf(f, "    for (int i = 0; i < %d; i++) {\n", n_iter);

    for (int b = 0; b < n_branches; b++) {
        fprintf(f, "        if (x < 10) x = 0;\n");
        for (int n = 0; n < n_nops; n++) {
            fprintf(f, "        __asm__(\"nop\");\n");
        }
    }

    fprintf(f, "    }\n");
    fprintf(f, "    return 0;\n");
    fprintf(f, "}\n");

    fclose(f);
    printf("Gerado: %s (%d branches, distancia %d, %d nops entre branches)\n",
           filename, n_branches, distancia, n_nops);
}

int main() {
    // Teste 1: 512 branches, variando distancia
    gerar_benchmark(512, 16,  10000000, "bench_512_d16.c");
    gerar_benchmark(512, 32,  10000000, "bench_512_d32.c");
    gerar_benchmark(512, 64,  10000000, "bench_512_d64.c");
    gerar_benchmark(512, 128, 10000000, "bench_512_d128.c");

    // Teste 2: 1024 branches (verificacao do tamanho)
    gerar_benchmark(1024, 16,  10000000, "bench_1024_d16.c");
    gerar_benchmark(1024, 32,  10000000, "bench_1024_d32.c");

    return 0;
}