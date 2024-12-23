#include "display.h"
#include "ports.h"
#include <stdint.h>
#include "../kernel/util.h"

//命令提示符(>)的字符宽度
#define CMD 2

void set_cursor(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char) (offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char) (offset & 0xff));
}

int get_cursor() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}


//获取相对当前行开头位置的偏移量
int get_inline_offset(){
    int offset = get_cursor();
    return offset % (MAX_COLS * 2);
}

//**用于移动光标的函数 */
bool move_cursor_left(char* key_buffer){
    int offset = get_cursor();
    if(get_inline_offset() <= CMD){
        return false;
    }
    set_cursor(offset - 2);
    return true;
}

bool move_cursor_right(char* key_buffer){
    int offset = get_cursor();
    int inline_offset = get_inline_offset();
    uint8_t length = string_length(key_buffer);
    if(inline_offset > length * 2 + CMD){
        return false;
    }
    set_cursor(offset + 2);
    return true;
}

int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

int get_row_from_offset(int offset) {
    return offset / (2 * MAX_COLS);
}

int move_offset_to_new_line(int offset) {
    return get_offset(0, get_row_from_offset(offset) + 1);
}

void set_char_at_video_memory(char character, int offset) {
    uint8_t *vidmem = (uint8_t *) VIDEO_ADDRESS;
    vidmem[offset] = character;
    vidmem[offset + 1] = WHITE_ON_BLACK;
}

char get_char_at_video_memory(int offset){
    //防止超出边界
    if(offset > MAX_COLS * MAX_ROWS){
        return '\0';
    }
    uint8_t *vidmem = (uint8_t *) VIDEO_ADDRESS;
    return vidmem[offset];
}

// 滚动屏幕
int scroll_ln(int offset) {
    memory_copy(
            (uint8_t * )(get_offset(0, 1) + VIDEO_ADDRESS),
            (uint8_t * )(get_offset(0, 0) + VIDEO_ADDRESS),
            MAX_COLS * (MAX_ROWS - 1) * 2
    );

    for (int col = 0; col < MAX_COLS; col++) {
        set_char_at_video_memory(' ', get_offset(col, MAX_ROWS - 1));
    }

    return offset - 2 * MAX_COLS;
}


void print_string(char *string) {
    int offset = get_cursor();
    int i = 0;
    while (string[i] != 0) {
        if (offset >= MAX_ROWS * MAX_COLS * 2) {
            offset = scroll_ln(offset);
        }
        if (string[i] == '\n') {
            offset = move_offset_to_new_line(offset);
        } else {
            set_char_at_video_memory(string[i], offset);
            offset += 2;
        }
        i++;
    }
    set_cursor(offset);
}

void print_nl() {
    int newOffset = move_offset_to_new_line(get_cursor());
    if (newOffset >= MAX_ROWS * MAX_COLS * 2) {
        newOffset = scroll_ln(newOffset);
    }
    set_cursor(newOffset);
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    for (int i = 0; i < screen_size; ++i) {
        set_char_at_video_memory(' ', i * 2);
    }
    set_cursor(get_offset(0, 0));
}


void print_backspace() {
    int newCursor = get_cursor() - 2;  // 获取当前光标位置并向前移两位（每个字符占2字节）

    // 1. 清空当前光标位置的字符
    set_char_at_video_memory(' ', newCursor);

    // 2. 向前移动后面的字符
    int len = CURRENT_OFFSET;  // 获取屏幕缓冲区当前有效的字符数量
    for (int i = newCursor; i < len * 2 - 2; i += 2) {
        char current_char = get_char_at_video_memory(i + 2);  // 获取后一个字符
        set_char_at_video_memory(current_char, i);  // 将后一个字符移动到当前字符位置
    }

    // 3. 清除最后一个字符
    set_char_at_video_memory(' ', len * 2 - 2);

    // 4. 更新光标位置
    set_cursor(newCursor);
}

