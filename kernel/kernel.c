#include "../fs/fs.h"          // 包含文件管理系统
#include "../drivers/display.h" // 屏幕显示功能
#include "../drivers/keyboard.h" // 键盘功能
#include "../cpu/isr.h"        // 中断处理
#include "../cpu/timer.h"      // 定时器
#include "util.h"              // 辅助函数
//#include "../fs/file.c"
//#include "../fs/bitmap.c"


void start_kernel() {
    clear_screen();
    print_string("Super-OS initialized.\n");

    // 初始化中断服务
    print_string("Initializing ISRs.\n");
    isr_install();

    // 启用外部中断
    print_string("Enabling external interrupts.\n");
    asm volatile("sti");


/**/

    // 初始化文件管理系统
    print_string("Initializing the file system.\n");
     ata_identify();
    init_sector_bitmap();       // 初始化位图
    init_root_directory();      // 初始化根目录

    // 初始化键盘
    print_string("Initializing keyboard (IRQ 1).\n");
    init_keyboard();

    // 打印欢迎信息
    clear_screen();
    print_string("Super-OS File System\n");
    print_string("> ");
}

/*
void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (compare_string(input, "CREATE") == 0) {
        // 创建文件
        print_string("Enter file name: ");
        char filename[32];
       
         filename[0]=1;
         
           //key_buffer[0] = '\0';
       // void test_input();
        //print_string("\n> ");
       // get_user_input(filename);
        print_string("Enter file size in bytes: ");
        char size_str[16];
         size_str[0]=1;
//get_user_input(size_str);
        uint32_t size = string_to_int(size_str);
        if (create_file(filename, size) == 0) {
            print_string("File created successfully!\n");
        } else {
            print_string("Failed to create file.\n");
        }
    } else if (compare_string(input, "WRITE") == 0) {
        // 写入文件
        //print_string("Enter file name: ");
        char filename[32];
        filename[0]=1;
        //get_user_input(filename);
        print_string("Enter data to write: ");
        char data[512];
        data[0]=12;
       // get_user_input(data);
        if (write_file(filename, data) == 0) {
            print_string("Data written successfully!\n");
        } else {
            print_string("Failed to write data.\n");
        }
    } else if (compare_string(input, "READ") == 0) {
        // 读取文件
        print_string("Enter file name: ");
        char filename[32];
        get_user_input(filename);
        char buffer[512];
        if (read_file(filename, buffer) == 0) {
            print_string("File content:\n");
            print_string(buffer);
            print_nl();
        } else {
            print_string("Failed to read file.\n");
        }
    } else if (compare_string(input, "DELETE") == 0) {
        // 删除文件
        print_string("Enter file name: ");
        char filename[32];
        get_user_input(filename);
        if (delete_file(filename) == 0) {
            print_string("File deleted successfully!\n");
        } else {
            print_string("Failed to delete file.\n");
        }
    } else if (compare_string(input, "LS") == 0) {
        // 列出目录内容
        print_string("Root directory:\n");
        list_directory();
    } else {
        print_string("Unknown command: ");
        print_string(input);
        print_string("\n> ");
    }
}
*/
void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (compare_string(input, "CREATE") == 0) {
        // 创建文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名

        print_string("Enter file size in bytes: ");
        char size_str[16];
        string_copy(size_str, "128"); // 模拟用户输入文件大小
        uint32_t size = string_to_int(size_str);

        if (create_file(filename, size) == 0) {
            print_string("File created successfully!\n");
        } else {
            print_string("Failed to create file.\n");
        }
    } else if (compare_string(input, "WRITE") == 0) {
        // 写入文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名

        print_string("Enter data to write: ");
        char data[512];
        string_copy(data, "Hello, this is a test."); // 模拟用户输入数据

        if (write_file(filename, data) == 0) {
            print_string("Data written successfully!\n");
        } else {
            print_string("Failed to write data.\n");
        }
    } else if (compare_string(input, "READ") == 0) {
        // 读取文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名

        char buffer[512];
        if (read_file(filename, buffer) == 0) {
            print_string("File content:\n");
            print_string(buffer);
            print_nl();
        } else {
            print_string("Failed to read file.\n");
        }
    } else if (compare_string(input, "DELETE") == 0) {
        // 删除文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名

        if (delete_file(filename) == 0) {
            print_string("File deleted successfully!\n");
        } else {
            print_string("Failed to delete file.\n");
        }
    } else if (compare_string(input, "LS") == 0) {
        // 列出目录内容
        print_string("Root directory:\n");
        list_directory();
    } else {
        print_string("Unknown command: ");
        print_string(input);
        print_string("\n> ");
    }
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