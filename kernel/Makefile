# Makefile

CROSS_COMPILER=i686-elf-
AS=$(CROSS_COMPILER)as
CC=$(CROSS_COMPILER)gcc
LD=$(CROSS_COMPILER)ld

CFLAGS=-std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
LDFLAGS=-T linker.ld -nostdlib

all: kernel.bin

kernel.bin: boot.o kernel.o
	$(LD) $(LDFLAGS) -o kernel.bin boot.o kernel.o

boot.o: boot.asm
	nasm -f elf32 boot.asm -o boot.o

kernel.o: kernel.cpp
	$(CC) $(CFLAGS) -c kernel.cpp -o kernel.o

clean:
	rm -f *.o kernel.bin
