#pragma once


#include <stdbool.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
static int CURRENT_OFFSET = 0;


#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5


void print_string(char* string);
void print_nl();
void clear_screen();
int scroll_ln(int offset);
void print_backspace();

int get_offset(int col, int row);
void set_char_at_video_memory(char character, int offset);
void set_cursor(int offset);


/* 光标控制 */
bool move_cursor_left(char* key_buffer);
bool move_cursor_right(char* key_buffer);