#!/bin/bash

DIR="$(cd "$(dirname "$0")" && pwd)"
LOGS="$DIR/logs"
mkdir -p "$LOGS"

for bin in "$DIR"/fase1_l* "$DIR"/fase2_l*; do
    [ -x "$bin" ] || continue
    [[ "$bin" == *.c ]] && continue
    [[ "$bin" == *.sh ]] && continue
    name="$(basename "$bin")"
    out="$LOGS/${name}_perf.txt"
    echo "===== $name =====" | tee "$out"
    perf stat -e branches,branch-misses "$bin" >> "$out" 2>&1
    echo "" >> "$out"
done

echo "Logs salvos em $LOGS/"
