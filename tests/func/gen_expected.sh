#!/bin/sh
set -eu

DIR="tests/func/cases"

for in_file in "$DIR"/*.in
do
    base="${in_file%.in}"

    out="${base}.expected.out"
    err="${base}.expected.err"
    code="${base}.expected.code"

    [ -f "$out" ] || : > "$out"
    [ -f "$err" ] || : > "$err"
    [ -f "$code" ] || printf "0\n" > "$code"
done
