#include <stdint.h>
#include <stdbool.h>
#include"../drivers/keyboard.h"
#include"../drivers/display.h"
#include "util.h"

//辅助内核编写的功能函数，一个函数，一个功能
void memory_copy(uint8_t *source, uint8_t *dest, uint32_t nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

//字符串长度
int string_length(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}
//逆转
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = string_length(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
//整数转换成字符串
void int_to_string(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

//增加字符串
void append(char s[], char n) {
    int len = string_length(s);
    s[len] = n;
    s[len + 1] = '\0';
}

//删除字符串的最后一个字符
//返回true: 字符串删除前不为空，可以删除
//返回false: 字符串删除前已经为空，不能再删除
bool backspace(char s[]) {
    int len = string_length(s);
    if (len > 0) {
        s[len - 1] = '\0';
        return true;
    } else {
        return false;
    }
}

//比较两个字符串,后期编写文件管理的各个指令时，可以用这种方式进行直接对比字符串。
//这个对比必须全符合
int compare_string(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

int compare_string_forward(char s1[],char s2[]){
    int i=0;
       while (s1[i] != '\0' && s1[i] != ' ') {
        i++;
    }
    for (int j = 0; j < i; j++) {
        if (s1[j] != s2[j]) {
            return -1;  
        }
    }
     return 0;

}

int string_to_int(const char *str) {
    int result = 0;
    int sign = 1; // 默认是正数

    // 检查负号
    if (*str == '-') {
        sign = -1;
        str++;
    }

    // 逐字符解析数字
    while (*str != '\0') {
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        } else {
            break; // 遇到非数字字符则停止
        }
        str++;
    }

    return result * sign;
}

void string_copy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void get_user_input(char *buffer) {
    int i = 0;
    while (1) {
        char key = get_key(); // 从键盘获取一个字符
        if (key == '\n') {   // 检测回车
            buffer[i] = '\0';
            break;
        } else if (key == '\b') { // 检测退格
            if (i > 0) {
                i--;
                print_backspace();
            }
        } else {
            buffer[i++] = key;
            char str[2] = {key, '\0'};
            print_string(str); // 在屏幕上打印用户输入
        }
    }
    print_string("\n ");
}

void print_int(int num) {
    char buffer[12]; // 最大支持 32 位整数（10 位数字 + 1 符号 + 1 结束符）
    int_to_string(num, buffer); // 调用已有的 int_to_string 函数
    print_string(buffer);       // 使用已有的 print_string 函数打印
}

/*

void test_input() {
    char key;

    
        key = get_key(); // 获取按键

        if (key != '\0') { // 忽略无效按键
            char str[2] = {key, '\0'};
            print_string("You pressed: ");
            print_string(str);
            print_string("\n");
        
    }
}
*/

void print_hex(uint32_t value) {
    // 打印前缀 "0x"
    print_string("0x");

    // 创建一个用于打印的字符数组
    char hex_str[9]; // 一个 32 位的十六进制数最大会有 8 位 + 1 字符串结束符
    hex_str[8] = '\0'; // 结束符

    // 用十六进制格式填充字符数组
    for (int i = 7; i >= 0; i--) {
        uint8_t hex_digit = value & 0xF;  // 获取最低4位
        if (hex_digit < 10) {
            hex_str[i] = '0' + hex_digit;  // 数字 0-9
        } else {
            hex_str[i] = 'A' + (hex_digit - 10);  // 字母 A-F
        }
        value >>= 4;  // 右移 4 位，处理下一个十六进制位
    }

    // 打印生成的十六进制字符串
    print_string(hex_str);
}

int split_string(const char *input, char result[MAX_TOKENS][MAX_TOKEN_LENGTH]) {
    int token_count = 0;  // 当前分割出的子字符串数量
    int token_length = 0; // 当前子字符串的长度
    int i = 0;            // 输入字符串的索引

    while (input[i] != '\0') { // 遍历整个字符串
        if (input[i] == ' ') { // 遇到空格，完成一个子字符串
            if (token_length > 0) { // 如果当前子字符串不为空
                result[token_count][token_length] = '\0'; // 添加字符串结束符
                token_count++; // 子字符串数量加一
                token_length = 0; // 重置当前子字符串长度
            }
        } else { // 非空格字符，添加到当前子字符串
            if (token_length < MAX_TOKEN_LENGTH - 1) { // 确保不会超出长度限制
                result[token_count][token_length] = input[i];
                token_length++;
            }
        }
        i++;
    }

    // 如果最后还有未结束的子字符串
    if (token_length > 0) {
        result[token_count][token_length] = '\0'; // 添加结束符
        token_count++;
    }

    return token_count; // 返回子字符串的数量
}