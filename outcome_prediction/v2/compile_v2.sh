#!/bin/bash

DIR="$(cd "$(dirname "$0")" && pwd)"
ERRORS=0

for src in "$DIR"/fase1_l*.c "$DIR"/fase2_l*.c; do
    name="$(basename "$src" .c)"
    echo -n "Compilando $name... "
    if gcc -Wall -Wextra -O0 "$src" -o "$DIR/$name"; then
        echo "OK"
    else
        echo "FALHOU"
        ERRORS=$((ERRORS + 1))
    fi
done

if [ "$ERRORS" -eq 0 ]; then
    echo "Todos compilados com sucesso."
else
    echo "$ERRORS arquivo(s) falharam."
    exit 1
fi
