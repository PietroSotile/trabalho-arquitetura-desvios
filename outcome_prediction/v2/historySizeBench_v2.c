/*
 * historySizeBench_v2.c
 *
 * Gerador de microbenchmarks para investigar o outcome predictor,
 * baseado na metodologia de Milenkovic et al. (2002).
 *
 * Melhorias em relação à v1:
 *  - Loop do-while em vez de for: gera um único branch condicional
 *    no final da iteração (jne ao decrementar), reduzindo a
 *    interferência no histórico global do preditor.
 *  - Contador decrementente (i--): evita a instrução de comparação
 *    extra que o for com i < liter gera; a flag zero do próprio
 *    decremento serve como condição de saída.
 *  - volatile int a: impede o compilador de eliminar os branches
 *    mesmo com -O0 (defesa extra).
 *  - __attribute__((optimize("O0"))): garante que a função do loop
 *    não seja otimizada independentemente das flags passadas ao gcc.
 *
 * Limitação remanescente:
 *  - O branch de saída do do-while ainda aparece no histórico global
 *    (1 branch por iteração em vez de 2 como no for). Isso é uma
 *    melhoria, mas não elimina completamente a interferência.
 *    Para fins acadêmicos, essa limitação deve ser mencionada
 *    no relatório.
 *
 * Compilação do gerador:
 *   gcc -O0 historySizeBench_v2.c -o bench_gen
 *   ./bench_gen
 *
 * Compilação dos benchmarks gerados:
 *   gcc -O0 fase1_l8.c -o fase1_l8
 *
 * Medição:
 *   perf stat -e branches,branch-misses ./fase1_l8
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * FASE 1 — Descobrir o comprimento máximo de padrão previsível.
 *
 * O spy branch executa o padrão: taken (l-1) vezes, not-taken 1 vez.
 * Aumentando l até a MPR começar a subir, encontramos o limite do
 * histórico. Quando MPR sobe em l=L, o histórico tem aproximadamente
 * L-1 bits (assumindo preditor local puro) ou 2*(L-1) bits (global).
 *
 * Uso do do-while:
 *   - Gera exatamente 1 branch por iteração (saída do loop)
 *     + 1 branch do spy = 2 branches por iteração no histórico global.
 *   - O for gerava 2 branches por iteração (teste de entrada + saída),
 *     totalizando 3 branches por iteração.
 */
void gerar_fase1(int l, long n_iter, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { printf("Erro ao abrir %s\n", filename); return; }

    fprintf(f, "#include <stdio.h>\n\n");
    fprintf(f, "__attribute__((optimize(\"O0\")))\n");
    fprintf(f, "int bench(long n_iter) {\n");
    fprintf(f, "    volatile int a = 0;\n");
    fprintf(f, "    long i = n_iter;\n");
    fprintf(f, "    do {\n");
    /* spy branch: not-taken 1 vez a cada l iteracoes */
    fprintf(f, "        if ((i %% %d) == 0) a = 0;\n", l);
    fprintf(f, "    } while (--i > 0);\n");
    fprintf(f, "    return a;\n");
    fprintf(f, "}\n\n");
    fprintf(f, "int main() {\n");
    fprintf(f, "    return bench(%ldL);\n", n_iter);
    fprintf(f, "}\n");

    fclose(f);
    printf("Gerado: %s  (l=%d, iter=%ld)\n", filename, l, n_iter);
}

/*
 * FASE 2 — Distinguir preditor local de global.
 *
 * Insere n_dummies branches "impossíveis" (i < 0, sempre not-taken)
 * antes do spy branch. Se o preditor for global, os dummies ocupam
 * slots no histórico e perturbam a previsão do spy:
 *   - MPR do spy aumenta  → preditor global confirmado
 *   - MPR do spy permanece baixo → preditor local confirmado
 *
 * Conforme o artigo, n_dummies deve ser 2*(L-1) para garantir que
 * nenhum outcome do spy permaneça no histórico global.
 *
 * Branches por iteração no histórico global com do-while:
 *   1 (saída do loop) + n_dummies + 1 (spy) = n_dummies + 2
 */
void gerar_fase2(int l, int n_dummies, long n_iter, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { printf("Erro ao abrir %s\n", filename); return; }

    fprintf(f, "#include <stdio.h>\n\n");
    fprintf(f, "__attribute__((optimize(\"O0\")))\n");
    fprintf(f, "int bench(long n_iter) {\n");
    fprintf(f, "    volatile int a = 0;\n");
    fprintf(f, "    long i = n_iter;\n");
    fprintf(f, "    do {\n");

    /* dummies: sempre not-taken, poluem o historico global */
    for (int d = 0; d < n_dummies; d++) {
        fprintf(f, "        if (i < 0) a = 1;\n");
    }

    /* spy branch */
    fprintf(f, "        if ((i %% %d) == 0) a = 0;\n", l);
    fprintf(f, "    } while (--i > 0);\n");
    fprintf(f, "    return a;\n");
    fprintf(f, "}\n\n");
    fprintf(f, "int main() {\n");
    fprintf(f, "    return bench(%ldL);\n", n_iter);
    fprintf(f, "}\n");

    fclose(f);
    printf("Gerado: %s  (l=%d, dummies=%d, iter=%ld)\n",
           filename, l, n_dummies, n_iter);
}

int main() {
    long n_iter = 10000000L;

    printf("=== Gerando fase 1 (l de 2 a 24) ===\n");
    for (int l = 2; l <= 24; l++) {
        char filename[64];
        sprintf(filename, "fase1_l%d.c", l);
        gerar_fase1(l, n_iter, filename);
    }

    /*
     * Fase 2: testar os valores de L que apresentaram picos na fase 1.
     * Para cada L de interesse, gerar com n_dummies = 2*(L-1) conforme
     * o artigo, e também com valores menores para ver a transição.
     *
     * Picos observados nos dados: L=8, L=11, L=18.
     * Geramos fase 2 para todos os L para comparação completa.
     */
    printf("\n=== Gerando fase 2 (l de 2 a 24, dummies = 2*(l-1)) ===\n");
    for (int l = 2; l <= 24; l++) {
        int n_dummies = 2 * (l - 1);
        char filename[64];
        sprintf(filename, "fase2_l%d.c", l);
        gerar_fase2(l, n_dummies, n_iter, filename);
    }

    printf("\nDica de compilacao em lote:\n");
    printf("  for f in fase1_l*.c fase2_l*.c; do\n");
    puts("    gcc -O0 $f -o ${f%.c}");
    puts("  done");
    printf("  done\n\n");
    printf("Dica de medicao em lote:\n");
    printf("  for b in fase1_l* fase2_l*; do\n");
    printf("    [ -x \"$b\" ] || continue\n");
    printf("    echo \"===== $b =====\"\n");
    printf("    perf stat -e branches,branch-misses ./$b 2>&1\n");
    printf("  done\n");

    return 0;
}
