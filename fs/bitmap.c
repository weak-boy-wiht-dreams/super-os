#include "fs.h"  // 包含文件系统全局定义和声明

// 位图数组，用于记录磁盘扇区的使用状态
uint8_t sector_bitmap[MAX_SECTORS / 8];

// 初始化位图，将所有扇区标记为空闲
void init_sector_bitmap() {
    for (int i = 0; i < MAX_SECTORS / 8; i++) {
        sector_bitmap[i] = 0; // 将每个字节置为 0，表示所有比特为 0（扇区空闲）
    }
}

// 分配指定数量的扇区
uint32_t allocate_sectors(uint32_t count) {
    uint32_t start_sector = 8; // 从扇区 8 开始分配，前 8 个扇区保留
    for (uint32_t i = start_sector; i < MAX_SECTORS; i++) {
        // 如果找到空闲扇区
        if (!(sector_bitmap[i / 8] & (1 << (i % 8)))) {
            sector_bitmap[i / 8] |= (1 << (i % 8)); // 标记该扇区为已分配
            count--; // 剩余需要分配的扇区数减一
            if (count == 0) {
                return i - count; // 返回起始扇区编号
            }
        }
    }
    return -1; // 无足够空闲扇区
}

// 释放从指定起始扇区开始的多个扇区
void free_sectors(uint32_t start, uint32_t count) {
    for (uint32_t i = start; i < start + count; i++) {
        sector_bitmap[i / 8] &= ~(1 << (i % 8)); // 标记为空闲
    }
}
