#include "../drivers/ports.h"  // 包含 I/O 端口读写的函数
#include "fs.h"               // 文件系统的全局定义和函数声明

// 定义 ATA 主通道的硬件寄存器端口地址
#define ATA_PRIMARY_DATA       0x1F0  // 数据寄存器，用于数据传输
#define ATA_PRIMARY_SECCOUNT   0x1F2  // 扇区数量寄存器，指定要传输的扇区数
#define ATA_PRIMARY_LBA_LOW    0x1F3  // LBA 地址的低 8 位
#define ATA_PRIMARY_LBA_MID    0x1F4  // LBA 地址的中间 8 位
#define ATA_PRIMARY_LBA_HIGH   0x1F5  // LBA 地址的高 8 位
#define ATA_PRIMARY_DRIVE      0x1F6  // 驱动器选择寄存器，选择主盘/从盘，并启用 LBA 模式
#define ATA_PRIMARY_COMMAND    0x1F7  // 命令寄存器，用于发送硬盘读写命令
#define ATA_PRIMARY_STATUS     0x1F7  // 状态寄存器，用于检查硬盘状态

// 定义 ATA 的命令和状态位
#define ATA_CMD_READ_SECTORS   0x20   // 读取扇区命令
#define ATA_CMD_WRITE_SECTORS  0x30   // 写入扇区命令
#define ATA_STATUS_BUSY        0x80   // 状态寄存器的 BUSY 位，表示硬盘忙
#define ATA_STATUS_DRQ         0x08   // 状态寄存器的 DRQ 位，表示数据准备好

// 等待硬盘不忙（BUSY 位为 0）


#include "../drivers/display.h"  // 假设你已经有了这个头文件来进行屏幕打印
void ata_wait_busy() {
    while (port_byte_in(ATA_PRIMARY_STATUS) & ATA_STATUS_BUSY);  // 读取状态寄存器，检查 BUSY 位是否为 1
}

#define ATA_CMD_IDENTIFY 0xEC  // 硬盘识别命令


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
//判断硬盘是否存在。
void ata_identify() {
    print_string("Starting ATA identify...\n");

    port_byte_out(ATA_PRIMARY_DRIVE, 0xA0);  // 选择主盘
    port_byte_out(ATA_PRIMARY_COMMAND, ATA_CMD_IDENTIFY);

    uint8_t status = port_byte_in(ATA_PRIMARY_STATUS);
    print_string("Initial Status: ");
    print_hex(status);
    print_string("\n");

    if (status == 0x00) {
        print_string("No device connected.\n");
        return;
    }

    while (status & ATA_STATUS_BUSY) {
        status = port_byte_in(ATA_PRIMARY_STATUS);
        print_string("Waiting for device... Status: ");
        print_hex(status);
        print_string("\n");
    }

    if (status & ATA_STATUS_DRQ) {
        print_string("Device identified!\n");
        return;
    }

    print_string("Device not responding.\n");

}

/*
调试该端口能否正常用,首先port_byte_out确定能用的，反复确认过了。问题依然是ATA接口的问题。
void ata_wait_drq() {
 
    uint8_t test_value = 0x55; // 测试写入的值（0x55 = 01010101）
    
    // 向 TEST_PORT 写入值
    port_byte_out(0x60, test_value);

    // 从 TEST_PORT 读取值
    uint8_t result = port_byte_in( 0x60);

    // 打印写入和读取的值
    print_string("Testing port_byte_out and port_byte_in:\n");
    print_string("Written value: ");
    print_hex(test_value);
    print_string("\nRead value: ");
    print_hex(result);
    print_string("\n");
 uint8_t status = port_byte_in(ATA_PRIMARY_STATUS);
    print_string("ATA Primary Status: ");
    print_hex(status);
    print_string("\n");
     while (!(port_byte_in(ATA_PRIMARY_STATUS) & ATA_STATUS_DRQ));
}
*/






// 等待硬盘的数据请求（DRQ 位为 1），表示数据准备好

void ata_wait_drq() {
     //print_string("444444 ");
    while (!(port_byte_in(ATA_PRIMARY_STATUS) & ATA_STATUS_DRQ));  // 读取状态寄存器，检查 DRQ 位是否为 1
}

// 从硬盘读取一个扇区
void ata_read_sector(uint32_t lba, uint8_t *buffer) {
    //print_string("666666 ");这里能跑通
    ata_wait_busy(); // 等待硬盘空闲,能正常读取
    //print_string("555555 ");
    // 设置要读取的扇区数量为 1
    port_byte_out(ATA_PRIMARY_SECCOUNT, 1);

    // 设置 LBA 地址的各部分
    port_byte_out(ATA_PRIMARY_LBA_LOW, (uint8_t)(lba & 0xFF));         // 设置 LBA 的低 8 位
    port_byte_out(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));  // 设置 LBA 的中 8 位
    port_byte_out(ATA_PRIMARY_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));// 设置 LBA 的高 8 位

    // 设置驱动器选择寄存器：启用 LBA 模式，并选择主盘/从盘
    port_byte_out(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));  // LBA 的高 4 位

    // 发送读取命令
    port_byte_out(ATA_PRIMARY_COMMAND, ATA_CMD_READ_SECTORS);
    print_string("444444 ");
    ata_wait_drq(); // 等待硬盘的数据准备好（DRQ 位为 1）
    
    // 从数据寄存器读取 512 字节数据（每次读取 2 字节，共 256 次循环）
    for (int i = 0; i < 256; i++) {
        ((uint16_t *)buffer)[i] = port_word_in(ATA_PRIMARY_DATA); // 每次从数据端口读取 2 字节
    }
    uint8_t status = port_byte_in(ATA_PRIMARY_STATUS);

//print_hex(status);
//print_nl();
}

// 向硬盘写入一个扇区
void ata_write_sector(uint32_t lba, uint8_t *buffer) {
    ata_wait_busy(); // 等待硬盘空闲

    // 设置要写入的扇区数量为 1
    port_byte_out(ATA_PRIMARY_SECCOUNT, 1);

    // 设置 LBA 地址的各部分
    port_byte_out(ATA_PRIMARY_LBA_LOW, (uint8_t)(lba & 0xFF));         // 设置 LBA 的低 8 位
    port_byte_out(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));  // 设置 LBA 的中 8 位
    port_byte_out(ATA_PRIMARY_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));// 设置 LBA 的高 8 位

    // 设置驱动器选择寄存器：启用 LBA 模式，并选择主盘/从盘
    port_byte_out(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));  // LBA 的高 4 位

    // 发送写入命令
    port_byte_out(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_SECTORS);

    ata_wait_drq(); // 等待硬盘的数据准备好（DRQ 位为 1）

    // 将 512 字节数据写入数据寄存器（每次写入 2 字节，共 256 次循环）
    for (int i = 0; i < 256; i++) {
        port_word_out(ATA_PRIMARY_DATA, ((uint16_t *)buffer)[i]); // 每次向数据端口写入 2 字节
    }

    ata_wait_busy(); // 等待写入完成（BUSY 位为 0）
}
