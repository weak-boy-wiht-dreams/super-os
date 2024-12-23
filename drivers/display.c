#include "display.h"           // 显示相关的头文件
#include "ports.h"             // 端口输入/输出操作的头文件
#include <stdint.h>
#include "../kernel/util.h"    // 内核常用的工具函数

// 命令提示符(>)的字符宽度
#define CMD 2

/**
 * 设置光标的位置
 * @param offset 偏移量，以字节为单位（一个字符占2字节）
 */
void set_cursor(int offset) {
    offset /= 2;  // 将字节偏移量转换为字符偏移量
    port_byte_out(REG_SCREEN_CTRL, 14);  // 设置高字节
    port_byte_out(REG_SCREEN_DATA, (unsigned char) (offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);  // 设置低字节
    port_byte_out(REG_SCREEN_DATA, (unsigned char) (offset & 0xff));
}

/**
 * 获取光标的当前位置
 * @return 光标的字节偏移量
 */
int get_cursor() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;  // 获取高字节
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);          // 获取低字节
    return offset * 2;  // 返回字节偏移量
}

/**
 * 获取当前行中光标的相对偏移量
 * @return 行内偏移量
 */
int get_inline_offset() {
    int offset = get_cursor();
    return offset % (MAX_COLS * 2);
}

/**
 * 光标向左移动
 * @param key_buffer 键盘输入缓冲区
 * @return 如果移动成功返回 true，否则返回 false
 */
bool move_cursor_left(char* key_buffer) {
    int offset = get_cursor();
    if (get_inline_offset() <= CMD) {  // 防止移动到命令提示符之前
        return false;
    }
    set_cursor(offset - 2);
    return true;
}

/**
 * 光标向右移动
 * @param key_buffer 键盘输入缓冲区
 * @return 如果移动成功返回 true，否则返回 false
 */
bool move_cursor_right(char* key_buffer) {
    int offset = get_cursor();
    int inline_offset = get_inline_offset();
    uint8_t length = string_length(key_buffer);  // 获取缓冲区长度
    if (inline_offset > length * 2 + CMD) {  // 防止移动到字符尾部之后
        return false;
    }
    set_cursor(offset + 2);
    return true;
}

/**
 * 获取某行某列的偏移量
 * @param col 列号
 * @param row 行号
 * @return 偏移量（以字节为单位）
 */
int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

/**
 * 根据偏移量获取所在的行号
 * @param offset 偏移量
 * @return 行号
 */
int get_row_from_offset(int offset) {
    return offset / (2 * MAX_COLS);
}

/**
 * 将偏移量移动到下一行的起始位置
 * @param offset 当前偏移量
 * @return 下一行的偏移量
 */
int move_offset_to_new_line(int offset) {
    return get_offset(0, get_row_from_offset(offset) + 1);
}

/**
 * 在视频内存的指定位置设置字符
 * @param character 要显示的字符
 * @param offset 字节偏移量
 */
void set_char_at_video_memory(char character, int offset) {
    uint8_t *vidmem = (uint8_t *) VIDEO_ADDRESS;
    vidmem[offset] = character;  // 设置字符
    vidmem[offset + 1] = WHITE_ON_BLACK;  // 设置颜色
}

/**
 * 获取视频内存指定位置的字符
 * @param offset 字节偏移量
 * @return 字符
 */
char get_char_at_video_memory(int offset) {
    // 防止超出屏幕边界
    if (offset > MAX_COLS * MAX_ROWS * 2) {
        return '\0';
    }
    uint8_t *vidmem = (uint8_t *) VIDEO_ADDRESS;
    return vidmem[offset];
}

/**
 * 滚动屏幕
 * @param offset 当前光标位置
 * @return 更新后的光标位置
 */
int scroll_ln(int offset) {
    memory_copy(
        (uint8_t *)(get_offset(0, 1) + VIDEO_ADDRESS),
        (uint8_t *)(get_offset(0, 0) + VIDEO_ADDRESS),
        MAX_COLS * (MAX_ROWS - 1) * 2
    );

    // 清空最后一行
    for (int col = 0; col < MAX_COLS; col++) {
        set_char_at_video_memory(' ', get_offset(col, MAX_ROWS - 1));
    }

    return offset - 2 * MAX_COLS;  // 更新光标位置
}

/**
 * 在屏幕上打印字符串
 * @param string 要打印的字符串
 */
void print_string(char *string) {
    int offset = get_cursor();
    int i = 0;
    while (string[i] != 0) {
        if (offset >= MAX_ROWS * MAX_COLS * 2) {  // 如果超出屏幕，则滚动
            offset = scroll_ln(offset);
        }
        if (string[i] == '\n') {  // 如果遇到换行符
            offset = move_offset_to_new_line(offset);
        } else {
            set_char_at_video_memory(string[i], offset);
            offset += 2;  // 移动到下一个字符位置
        }
        i++;
    }
    set_cursor(offset);  // 更新光标位置
}

/**
 * 打印换行
 */
void print_nl() {
    int newOffset = move_offset_to_new_line(get_cursor());
    if (newOffset >= MAX_ROWS * MAX_COLS * 2) {  // 如果超出屏幕，则滚动
        newOffset = scroll_ln(newOffset);
    }
    set_cursor(newOffset);
}

/**
 * 清空屏幕
 */
void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    for (int i = 0; i < screen_size; ++i) {
        set_char_at_video_memory(' ', i * 2);
    }
    set_cursor(get_offset(0, 0));  // 重置光标到屏幕左上角
}

/**
 * 实现退格键功能
 */
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
