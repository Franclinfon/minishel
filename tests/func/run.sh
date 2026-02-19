#!/bin/sh
set -eu

MSH=../../src/minishell

run_case()
{
    name="$1"
    in="cases/$name.in"
    exp="cases/$name.expected"

    out="$(mktemp)"
    err="$(mktemp)"
    code_file="$(mktemp)"

    if [ "${VALGRIND:-0}" = "1" ]; then
        valgrind --leak-check=full --error-exitcode=42 "$MSH" < "$in" >"$out" 2>"$err" || true
        rc="$?"
    else
        "$MSH" < "$in" >"$out" 2>"$err" || true
        rc="$?"
    fi

    printf "%s\n" "$rc" > "$code_file"

    diff -u "$exp.out" "$out"
    diff -u "$exp.err" "$err"
    diff -u "$exp.code" "$code_file"

    rm -f "$out" "$err" "$code_file"
}

for f in cases/*.in
do
    base="$(basename "$f" .in)"
    run_case "$base"
done
