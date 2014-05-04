KERNEL_OFFSET=-Ttext 0xc400
LD_ARCH=-m elf_i386
LD_FORMAT=--oformat binary
NASM_ARCH=-f elf32
GCC_ARCH=-m32
GCC_FLAG=-ffreestanding

all: slef.img

slef.img: boot.img kernel.sys
	bash makeimg.sh

kernel.sys: kernel_entry.o 
	cp kernel_entry.o kernel_entry.sys
	#ld -o $@ $(LD_ARCH) $(KERNEL_OFFSET) $(LD_FORMAT) $^
	#ld -o $@ $(LD_ARCH) /lib/libc.so $(KERNEL_OFFSET) $(LD_FORMAT) $^


kernel_entry.o: kernel_entry.asm
	nasm $< -f bin -o $@

boot.img: boot.asm
	nasm $< -f bin -o $@

clean:
	rm *.o *.img
