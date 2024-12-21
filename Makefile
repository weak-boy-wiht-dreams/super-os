# $@ = target file
# $< = first dependency
# $^ = all dependencies

# Detect all .c source files in kernel, drivers, cpu, and fs directories
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c fs/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h fs/*.h)
OBJ_FILES = ${C_SOURCES:.c=.o cpu/interrupt.o}

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Build the kernel binary
kernel.bin: boot/kernel_entry.o ${OBJ_FILES}
	i686-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

# Build the OS image by concatenating the MBR and kernel binary
os-image.bin: boot/mbr.bin kernel.bin
	cat $^ > $@

# Run the OS in QEMU
run: os-image.bin
	qemu-img create my_disk.img 10M

	qemu-system-i386 -fda $< -hda my_disk.img
#qemu-system-i386 -fda $<

# Display the OS image content in a hex dump
echo: os-image.bin
	xxd $<

# Only for debugging purposes, create an ELF file
kernel.elf: boot/kernel_entry.o ${OBJ_FILES}
	i686-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^

# Start the OS in debug mode with QEMU and connect with GDB
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
	i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Compile C source files into object files
%.o: %.c ${HEADERS}
	i686-elf-gcc -g -m32 -ffreestanding -c $< -o $@ # -g for debugging

# Assemble .asm files into object files
%.o: %.asm
	nasm $< -f elf -o $@

# Assemble .asm files into binary files
%.bin: %.asm
	nasm $< -f bin -o $@

# Disassemble binary files into human-readable assembly
%.dis: %.bin
	ndisasm -b 32 $< > $@

# Clean up all generated files
clean:
	$(RM) *.bin *.o *.dis *.elf
	$(RM) kernel/*.o
	$(RM) boot/*.o boot/*.bin
	$(RM) drivers/*.o
	$(RM) cpu/*.o
	$(RM) fs/*.o
	$(RM) my_disk.img
