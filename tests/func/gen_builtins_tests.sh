#!/bin/sh
set -eu

DIR="tests/func/cases"
mkdir -p "$DIR"

create_empty() {
    : > "$1"
}

create_code() {
    printf "%s\n" "$2" > "$1"
}

echo "Generating builtin tests..."

# 050-echo
printf "echo hello\n" > "$DIR/050-echo.in"
printf "hello\n" > "$DIR/050-echo.expected.out"
create_empty "$DIR/050-echo.expected.err"
create_code "$DIR/050-echo.expected.code" "0"

# 051-echo-n
printf "echo -n hello; echo X\n" > "$DIR/051-echo-n.in"
printf "helloX\n" > "$DIR/051-echo-n.expected.out"
create_empty "$DIR/051-echo-n.expected.err"
create_code "$DIR/051-echo-n.expected.code" "0"

# 052-cd
printf "cd /; /bin/pwd\n" > "$DIR/052-cd.in"
printf "/\n" > "$DIR/052-cd.expected.out"
create_empty "$DIR/052-cd.expected.err"
create_code "$DIR/052-cd.expected.code" "0"

# 053-exit
printf "exit 42\necho nope\n" > "$DIR/053-exit.in"
create_empty "$DIR/053-exit.expected.out"
create_empty "$DIR/053-exit.expected.err"
create_code "$DIR/053-exit.expected.code" "42"

# 054-kill-bad
printf "kill -9\n" > "$DIR/054-kill-bad.in"
create_empty "$DIR/054-kill-bad.expected.out"
create_empty "$DIR/054-kill-bad.expected.err"
create_code "$DIR/054-kill-bad.expected.code" "1"

echo "Builtin tests generated ✔"
