#!/bin/bash

DIR="$(cd "$(dirname "$0")" && pwd)"
OUT="$DIR/resultadoPerfFase1.txt"

> "$OUT"

for bin in "$DIR"/fase1_l*; do
    [ -x "$bin" ] && [ ! "${bin##*.}" = "c" ] && [ ! "${bin##*.}" = "sh" ] || continue
    name="$(basename "$bin")"
    echo "===== $name =====" | tee -a "$OUT"
    perf stat -e branches,branch-misses "$bin" >> "$OUT" 2>&1
    echo "" >> "$OUT"
done

echo "Resultados salvos em $OUT"
