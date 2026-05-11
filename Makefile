all: crunchos.bin

boot/boot.bin: boot/boot.asm
	nasm -f bin boot/boot.asm -o boot/boot.bin

kernel/kernel.o: kernel/kernel.c
	gcc -m32 -ffreestanding -fno-pie -c kernel/kernel.c -o kernel/kernel.o

kernel/kernel.bin: kernel/kernel.o
	ld -m elf_i386 -Ttext 0x1000 --oformat binary -o kernel/kernel.bin kernel/kernel.o

crunchos.bin: boot/boot.bin kernel/kernel.bin
	cat boot/boot.bin kernel/kernel.bin > crunchos.bin

run: crunchos.bin
	qemu-system-x86_64 -drive format=raw,file=crunchos.bin -nographic

clean:
	rm -f boot/boot.bin kernel/kernel.o kernel/kernel.bin crunchos.bin
