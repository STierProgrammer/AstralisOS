# How to build stierium

stierium currently only support x86-64.

## Prerequisites

Make sure you have installed the following tools:

* QEMU
* xorriso
* GNU Make
* NASM

you should also have a cross compiler.

## Configuration

Before building, you need to configure make.conf in the root.

ARCH - Architecture that you're building for. 
BOOTLDR - Bootloader you want to use.
PROTOCOL - Boot protocol that you want to use.

## Building

From the root, run:

`make` and for running you can run `make run`

