#!/bin/sh

set -xe

gcc -m64 \
    -ffreestanding \
    -nostdlib \
    -static \
    -fno-stack-protector \
    -no-pie \
    -Wl,-e,_start \
    test_program.c \
    -o test_program

rm ../initrd/test_program
mv ./test_program ../initrd/
