KERNEL_OFFSET=-Ttext 0x1200
LD_ARCH=-m elf_i386
LD_FORMAT=--oformat binary
NASM_ARCH=-f elf32
GCC_ARCH=-m32
GCC_FLAG=-ffreestanding

all: slef.img

slef.img: boot.bin fill.bin kernel.bin fillend.bin
	cat $^ > $@

kernel.bin: kernel_entry.o asmfunc.o kernel.o
	ld -o $@ $(LD_ARCH) $(KERNEL_OFFSET) $(LD_FORMAT) $^
	#ld -o $@ $(LD_ARCH) /lib/libc.so $(KERNEL_OFFSET) $(LD_FORMAT) $^

kernel.s: kernel.c
	gcc $(GCC_FLAG) -S -o kernel.s -c kernel.c

kernel.o: kernel.c kernel.s
	gcc $(GCC_FLAG) -c $< $(GCC_ARCH) -o $@

kernel_entry.o: kernel_entry.asm
	nasm $< $(NASM_ARCH) -o $@

boot.bin: boot.asm
	nasm $< -f bin -o $@

fill.bin: fill.asm
	nasm $< -f bin -o $@

fillend.bin: fillend.asm
	nasm $< -f bin -o $@

asmfunc.o: asmfunc.asm
	nasm $< $(NASM_ARCH) -o $@

clean:
	rm *.bin *.o *.img
