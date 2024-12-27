#include "keyboard.h"
#include "ports.h"
#include "../cpu/isr.h"
#include "display.h"
#include "../kernel/util.h"
#include <stdint.h>
#include <stdbool.h>
#include "../kernel/kernel.h"

//退格键扫描码
#define BACKSPACE 0x0E
//回车键扫描码1
#define ENTER 0x1C

//左方向键扫描码
#define EXTENDED_LEFT_ARROW 0x4B
//右方向键
#define EXTENDED_RIGHT_ARROW 0x4D

//字符缓冲区void print_hex(uint32_t value)
#define SC_MAX 57

const char *sc_name[] = {"ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                         "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                         "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                         "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                         "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                         "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

//定义扫描码到 ASCII 字符的映射关系
const char sc_ascii[] = {'?', '?', '1', '2', '3', '4', '5', '6',
                         '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
                         'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G',
                         'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
                         'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

//存储当前输入位置
int INPUT_POS = 0;

//处理普通键与功能键盘
static void keyboard_callback(registers_t *regs) {
    uint16_t scancode = port_byte_in(0x60);
    uint32_t extended_scancode;
    bool is_extended = false;

    if(scancode == 0xE0){  //应该没有其他占用
        is_extended = true;
        extended_scancode = port_byte_in(0x60);
    }

    if (scancode > SC_MAX && !is_extended) return;

    if(!is_extended){
        if (scancode == BACKSPACE) {
            if (at_buffer_end(key_buffer) && backspace(key_buffer)) {
                print_backspace();
                //向左移动输入位置
                set_input_pos(INPUT_POS - 1);
            }
            //&& !other_mode
        } else if (scancode == ENTER ) {
            print_nl();
            execute_command(key_buffer);
            key_buffer[0] = '\0';
            set_input_pos(0);

        }/*
        else if(scancode == ENTER&&other_mode){
         print_nl();
         key_buffer[0] = '\0';

        }*/
        else {
            char letter = sc_ascii[(int) scancode];
            set_letter_at_pos(key_buffer, INPUT_POS, letter);
            char str[2] = {letter, '\0'};
            //print_string("a");
            print_string(str);
        }
    }else{
        if(extended_scancode == EXTENDED_LEFT_ARROW){
            if(move_cursor_left(key_buffer)) set_input_pos(INPUT_POS - 1);
        }else if(extended_scancode == EXTENDED_RIGHT_ARROW){
            if(move_cursor_right(key_buffer)) set_input_pos(INPUT_POS + 1);
        } 
    }
}

//将特定中断号与处理函数绑定，确保当键盘中断发生时，调用 keyboard_callback
void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}

//获得字符
char get_key() {
    // 从键盘端口读取扫描码
    uint8_t scancode = port_byte_in(0x60);

    // 转换扫描码为 ASCII 字符
    char letter = sc_ascii[(int) scancode];

    // 返回对应的字符
    return letter;
}

// 设置字符串中某个位置的字符，若位置超过总长度-1, 则失败; 若为总长度-1, 则添加; 其他情况修改对应位置
void set_letter_at_pos(char* buffer, int pos, char letter) {
    int string_length_full = string_length(buffer);
    if (pos == string_length_full) {
        // 如果位置正好是字符串末尾，添加字符
        append(buffer, letter);
    } else if (pos >= string_length_full) {
        // 如果位置超出了当前字符串长度，返回，不做任何操作
        return;
    } else {
        // 其他情况下，修改对应位置的字符
        buffer[pos] = letter;
    }
    
    set_input_pos(INPUT_POS + 1);
}

bool at_buffer_end(char* buffer){
    return INPUT_POS >= string_length(buffer) - 1;
}


// 在移动光标位置后，相应移动输入位置
void set_input_pos(int pos) {
    INPUT_POS = pos;
}

char get_user_input(){




    
}
