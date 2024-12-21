#include <stdint.h>


unsigned char port_byte_in(uint16_t port) {
    unsigned char result;
    /* %%dx：这是目标端口号，传递给 dx 寄存器。端口号是一个 16 位的值，在此函数中由 port 参数传递给 dx 寄存器。
%%al：这是从端口读取的数据将存储的寄存器。al 是 eax 寄存器的低 8 位部分，用于存储输入的数据。
in %%dx, %%al：表示从由 dx 寄存器指定的端口读取一个字节，并将其存入 al 寄存器中
     
     */
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));/*"=a" (result)：这表示将 al 寄存器的值（输入数据）传递给 C 语言变量 result。=a 表示将 al 寄存器的值返回给 C 语言中的 result 变量。
"d" (port)：这表示将 port 变量的值传递给 dx 寄存器，即将端口号放入 dx 寄存器中*/
    return result;
}

void port_byte_out(uint16_t port, uint8_t data) {
    asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short port_word_in(uint16_t port) {
    unsigned short result;
    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out(uint16_t port, uint16_t data) {
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}
