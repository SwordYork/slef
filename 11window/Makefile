KERNEL_OFFSET=-Ttext 0xc400
LD_ARCH=-m elf_i386
LD_FORMAT=--oformat binary
NASM_ARCH=-f elf32
GCC_ARCH=-m32
GCC_FLAG=-ffreestanding

all: slef.img

slef.img: boot.img kernel.sys
	bash makeimg.sh

kernel.sys: kernel_entry.o asmfunc.o kernel.o 
	ld -o $@ $(LD_ARCH) $(KERNEL_OFFSET) $(LD_FORMAT) $^

kernel.o: graphic.o dsctbl.o init.o int.o \
	itoa.o fifo.o mouse.o keyboard.o memory.o \
	sheet.o
	ld -o $@ $(LD_ARCH) -r $^  
 
%.o: %.c
	gcc $(GCC_FLAG) -c $*.c $(GCC_ARCH) -o $*.o

kernel_entry.o: kernel_entry.asm
	nasm $< $(NASM_ARCH) -o $@

boot.img: boot.asm
	nasm $< -f bin -o $@

asmfunc.o: asmfunc.asm
	nasm $< $(NASM_ARCH) -o $@

clean:
	rm *.img *.sys *.o 
