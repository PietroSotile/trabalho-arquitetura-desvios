#include <stdio.h>
#include <stdlib.h>



// Estamos tentando descobrir o tamanho do histórico do outcome predictor - se ele tem 4 bits, 8 bits, 16 bits, etc
// A ideia é: o preditor guarda os últimos N outcomes do branch num shift register. Se o padrão do branch for maior que N, o preditor não consegue distinguir todas as posições do padrão e começa a errar.
// O (i % l) == 0 cria um padrão de comprimento l - not taken uma vez a cada l execuções, taken todo o resto. Aumentando l até o MPR começar a subir, encontramos o limite do histórico.
// Quando o MPR sobe em l = L, sabemos que o histórico tem L-1 bits.

// O que tem que variar aqui é o l (onde l = numero de bits do preditor)
void gerar_fase1(int l, int n_iter, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { printf("Erro ao abrir %s\n", filename); return; }

    fprintf(f, "#include <stdio.h>\n\n");
    fprintf(f, "int main() {\n");
    fprintf(f, "    volatile int a = 0;\n");
    fprintf(f, "    long unsigned liter = %d;\n", n_iter);
    fprintf(f, "    for (long unsigned i = 0; i < liter; i++) {\n");
    fprintf(f, "        if ((i %% %d) == 0) a = 0;\n", l);
    fprintf(f, "    }\n");
    fprintf(f, "    return 0;\n");
    fprintf(f, "}\n");

    fclose(f);
    printf("Gerado: %s (l=%d)\n", filename, l);
}

// Essa fase consiste em adicionar dummy branches de modo a identificar se o outcome predictor é
// local ou global. Ao adicionar dummy branches, se for global após algum tempo o MPR deve começar a errar
// NOTA: o l do parametro é o l descoberto na fase 1 
void gerar_fase2(int l, int n_dummies, int n_iter, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { printf("Erro ao abrir %s\n", filename); return; }

    fprintf(f, "#include <stdio.h>\n\n");
    fprintf(f, "int main() {\n");
    fprintf(f, "    volatile int a = 0;\n");
    fprintf(f, "    long unsigned liter = %d;\n", n_iter);
    fprintf(f, "    for (long unsigned i = 0; i < liter; i++) {\n");

    for (int d = 0; d < n_dummies; d++) {
        fprintf(f, "        if (i < 0) a = 1;\n");
    }

    fprintf(f, "        if ((i %% %d) == 0) a = 0;\n", l);
    fprintf(f, "    }\n");
    fprintf(f, "    return 0;\n");
    fprintf(f, "}\n");

    fclose(f);
    printf("Gerado: %s (l=%d, dummies=%d)\n", filename, l, n_dummies);
}

int main() {
    // Fase 1 - achar L variando o padrao
    for (int l = 2; l <= 20; l++) {
        char filename[64];
        sprintf(filename, "fase1_l%d.c", l);
        gerar_fase1(l, 10000000, filename);
    }

    // // Fase 2 - inserir dummies pra distinguir local de global
    // // assumindo L=6 como exemplo, ajustar depois
    // int L = 6;
    // int n_dummies = 2 * (L - 1);
    // gerar_fase2(L, n_dummies, 10000000, "fase2_local_ou_global.c");

    return 0;
}