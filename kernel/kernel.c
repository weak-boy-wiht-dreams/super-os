#include "../fs/fs.h"          // 包含文件管理系统
#include "../drivers/display.h" // 屏幕显示功能
#include "../drivers/keyboard.h" // 键盘功能
#include "../cpu/isr.h"        // 中断处理

#include "../cpu/timer.h"      //定时器
#include "util.h"              // 辅助函数

#include "../sample-programs/snake.h"

//#include "../fs/file.c"
//#include "../fs/bitmap.c"

volatile bool should_start_game = false;
volatile uint8_t game_difficulty;

void print_cmd(){
    
    clear_screen();
    print_string("Super-OS File System\n");
    print_string("> ");
}

void start_kernel() {
    clear_screen();
    print_string("Super-OS initialized.\n");

    // 初始化中断服务
    print_string("Initializing ISRs.\n");
    isr_install();

    // 启用外部中断
    print_string("Enabling external interrupts.\n");
    asm volatile("sti");

    // 初始化文件管理系统
    print_string("Initializing the file system.\n");
    ata_identify();
    init_sector_bitmap();       // 初始化位图
    init_root_directory();      // 初始化根目录

    // 初始化键盘
    print_string("Initializing keyboard (IRQ 1).\n");
    init_keyboard();

    //初始化定时器
    print_string("Initializing Timer (IRQ 0).\n");
    init_timer(TIMER_FREQ);

    // 打印欢迎信息
    print_cmd();

    //添加内核主循环
    while(1) {
        if (should_start_game) {
            delay(3000);
            should_start_game = false;
            // 确保中断是启用的
            __asm__ volatile("sti");
            game_loop(game_difficulty);
            print_string("\n> ");
        }
        __asm__ volatile("hlt");
    }
}


void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (compare_string_forward(input, "CREATE") == 0) {
        // 创建文件

         char result[MAX_TOKENS][MAX_TOKEN_LENGTH];
         int count = split_string(input, result);
         for(int i=0;i<count;i++){
            print_string(result[i]);
         }

       // print_string("Enter file name: ");
        char filename[32];
        string_copy(filename,result[1] ); // 模拟用户输入文件名
        //print_nl();

        //print_string("Enter file size in bytes: (testfile)");
       // print_nl();
  
        char size_str[16];
        string_copy(size_str, result[2]); // 模拟用户输入文件大小
        uint32_t size = string_to_int(size_str);

        if (create_file(filename, size) == 0) {
            print_string("File created successfully!\n");
        } else {
            print_string("Failed to create file.\n");
        }
    } else if (compare_string_forward(input, "WRITE") == 0) {
        // 写入文件
        //print_string("Enter file name: ");
        char result[MAX_TOKENS][MAX_TOKEN_LENGTH];
         int count = split_string(input, result);
        char filename[32];
        string_copy(filename, result[1]); // 模拟用户输入文件名
        //print_nl();

        //print_string("Enter data to write: \n");
        char data[512];
        string_copy(data, result[2]); // 模拟用户输入数据

        if (write_file(filename, data) == 0) {
            print_string("Data written successfully!\n");
        } else {
            print_string("Failed to write data.\n");
        }
        print_string("\n> ");
        
    } else if (compare_string_forward(input, "READ") == 0) {

           char result[MAX_TOKENS][MAX_TOKEN_LENGTH];
         int count = split_string(input, result);
        // 读取文件
        //print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, result[1]); // 模拟用户输入文件名
        //print_nl();

        char buffer[512];
        if (read_file(filename, buffer) == 0) {
            print_string("File content:\n");
            print_string(buffer);
            print_nl();
        } else {
            print_string("Failed to read file.\n");
        }
        print_string("\n> ");
    } else if (compare_string_forward(input, "DELETE") == 0) {
    
        char result[MAX_TOKENS][MAX_TOKEN_LENGTH];
         int count = split_string(input, result);
       // print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, result[1]); // 模拟用户输入文件名print_string("\n> ");

        if (delete_file(filename) == 0) {
            print_string("File deleted successfully!\n");
        } else {
            print_string("Failed to delete file.\n");
        }
    } else if (compare_string(input, "LS") == 0) {
        // 列出目录内容
        print_string("Root directory:\n");
        list_directory();
    } else if (compare_string_forward(input, "SNAKE") == 0){
         char result[MAX_TOKENS][MAX_TOKEN_LENGTH];
         int count = split_string(input, result);

         should_start_game = true;
         game_difficulty = string_to_int(result[1]);
        
    } else if(compare_string(input, "HELP")==0){
     print_string("---you can create file like this :CREATE YOURFILE 12(your file size)");
     print_string("\n> "); 
     print_string("---you can write file like this :WRITE YOURFILE data(your file data)");
     print_string("\n> "); 
     print_string("---you can read file like this :READ YOURFILE ");
     print_string("\n> "); 
     print_string("---you can delete file like this :delete YOURFILE ");
     print_string("\n> "); 
     print_string("---you can play our interesting game by :SNAKE 1(0,1,2,3 to choose your difficulity) ");
     print_string("\n> ");
    }
    
    else {
        print_string("Unknown command: ");
        print_string(input);
    }

    print_string("\n> ");
 
}