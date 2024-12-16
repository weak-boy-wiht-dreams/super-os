#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/display.h"
#include "../drivers/keyboard.h"

#include "util.h"

void start_kernel() {
    clear_screen();
    print_string("Installing interrupt service routines (ISRs).\n");
    isr_install();

    print_string("Enabling external interrupts.\n");
    asm volatile("sti");

    print_string("Initializing keyboard (IRQ 1).\n");
    init_keyboard();

    clear_screen();
    print_string("> ");
}

void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    if(compare_string(input ,"CREATE")==0){
        print_string("crate txt");
    }
    print_string("Unknown command: ");
    print_string(input);
    print_string("\n> ");
}
/*这是我在qemu里写的自己的极简的os操作系统的目录：zero@zero-VirtualBox:~/super-os$ ls -R
.:
boot  cpu  drivers  kernel  kernel.bin  Makefile  os-image.bin

./boot:
disk.asm  gdt.asm  kernel_entry.asm  kernel_entry.o  mbr.asm  mbr.bin  print-16bit.asm  print-32bit.asm  switch-to-32bit.asm

./cpu:
idt.c  idt-gdt-cpu.txt  idt.h  idt.o  interrupt.asm  interrupt.o  isr.c  isr.h  isr.o  timer.c  timer.h  timer.o

./drivers:
display.c  display.h  display.o  keyboard.c  keyboard.h  keyboard.o  ports.c  ports.h  ports.o

./kernel:
kernel.c  kernel.o  util.c  util.h  util.o，目前我实现了bios的内核加载，gdt段描述符的定义，idt的简单的中断门描述，可以打印字符串和换行了，接下来准备实现文件管理系统，但在设计文件管理系统中，我不准备*/