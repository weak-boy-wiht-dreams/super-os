#pragma once

#include <stdbool.h>
#include <stdint.h>

static char key_buffer[256];  
//static char command_buffer[256];  //指令缓冲区，仅用于输入指令(在按enter时将key_buffer中的内容复制到command_buffer中)
static char keychain_buffer[256];  //键盘输入的缓冲区，所有程序都可使用

void init_keyboard();
char get_key();

void set_input_pos(int pos);
void set_chain_input_pos(int pos);

void append_key_to_buffer(uint16_t scancode, char* buffer, char* return_char, int input_pos, void (*input_pos_inc_func)(int));
void set_letter_at_pos(char* buffer, int pos, char letter, void (*input_pos_inc_func)(int));
void handle_backspace(char* buffer, int input_pos);
bool at_buffer_end(char* buffer);


char get_next_char_from_keychain_buffer();