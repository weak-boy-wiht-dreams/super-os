#include "../fs/fs.h"          // 包含文件管理系统
#include "../drivers/display.h" // 屏幕显示功能
#include "../drivers/keyboard.h" // 键盘功能
#include "../cpu/isr.h"        // 中断处理
#include "../cpu/timer.h"      // 定时器
#include "util.h"              // 辅助函数
//#include "../fs/file.c"
//#include "../fs/bitmap.c"
void split_input(char *input, char *output[]){
     int i=0,j=0,k=0;
     int nums =0;
     while(input[i]!='\0'){
          if(input[i]==' '||input[i]=='\0'){
          nums++;//一共有多少个指令（也就是子字符串）；
     }  
    i++;
         }

    for (int idx = 0; idx < nums; idx++) {
        output[idx] = (char *) &input[64* idx];  // 指向输入字符串中的不同位置
    }

    i=0;
     while(nums>0){
    // while(input[i]==' ') {
          //  i++;
          //  }
            j=0;
     while (input[i] != ' ' && input[i] != '\0') {
            output[k][j] = input[i];  // 将字符复制到 output[k]
            i++;
            j++;
        }
        output[k][j] = '\0';
            i++;
            k++;
            nums--;


}
 output[k] = '\0';

}


bool other_mode = false;
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


void execute_command(char *input) {
    if (compare_string(input, "EXIT") == 0) {
        print_string("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (compare_string_forward(input, "CREATE") == 0) {
        // 创建文件
         //other_mode =true;
         char result[MAX_TOKENS][MAX_TOKEN_LENGTH];
         int count = split_string(input, result);
         //char *output[10];
         //split_input(input,output);
         for(int i=0;i<count;i++){
            print_string(result[i]);

         }
       // print_string("Enter file name: ");
       // key_buffer[0] = '/0';
       // execute_command(input);
        char filename[32];
        string_copy(filename,result[1] ); // 模拟用户输入文件名
        print_nl();

        print_string("Enter file size in bytes: (testfile)");
        print_nl();
        char size_str[16];
        string_copy(size_str, result[2]); // 模拟用户输入文件大小
        uint32_t size = string_to_int(size_str);

        if (create_file(filename, size) == 0) {
            print_string("File created successfully!\n");
        } else {
            print_string("Failed to create file.\n");
        }
        print_string("\n> ");
    } else if (compare_string(input, "WRITE") == 0) {
        // 写入文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名
        print_nl();

        print_string("Enter data to write: \n");
        char data[512];
        string_copy(data, "Hello, this is a test."); // 模拟用户输入数据

        if (write_file(filename, data) == 0) {
            print_string("Data written successfully!\n");
        } else {
            print_string("Failed to write data.\n");
        }
        print_string("\n> ");
        
    } else if (compare_string(input, "READ") == 0) {
        // 读取文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名
        print_nl();

        char buffer[512];
        if (read_file(filename, buffer) == 0) {
            print_string("File content:\n");
            print_string(buffer);
            print_nl();
        } else {
            print_string("Failed to read file.\n");
        }
        print_string("\n> ");
    } else if (compare_string(input, "DELETE") == 0) {
        // 删除文件
        print_string("Enter file name: ");
        char filename[32];
        string_copy(filename, "testfile"); // 模拟用户输入文件名print_string("\n> ");

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

void create_command(char*input,char*filename,uint32_t*size){
     if(compare_string_forward(input,"CREATE")!=0)
    {
        print_string("No such rules!");
    }

else {



     }

}



