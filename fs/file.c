#include "fs.h"  // 包含文件系统的全局定义和声明
//#include"directory.c"
// 创建一个新文件
int create_file(const char *filename, uint32_t size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!root_directory.entries[i].is_used) { // 找到空闲条目
            root_directory.entries[i].is_used = 1; // 标记为已使用
            string_copy(root_directory.entries[i].filename, filename); // 设置文件名
            root_directory.entries[i].size = size; // 设置文件大小
            root_directory.entries[i].start_sector = allocate_sectors((size + 511) / 512); // 分配扇区
            root_directory.entries[i].is_directory = 0; // 标记为文件
            ata_write_sector(3, (uint8_t *)&root_directory); // 写回根目录表
            return 0; // 成功创建
        }
    }
    return -1; // 无法创建文件
}

// 写入文件内容
int write_file(const char *filename, const char *data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_directory.entries[i].is_used &&
            compare_string(root_directory.entries[i].filename, filename) == 0) {
            uint32_t start_sector = root_directory.entries[i].start_sector;
            uint32_t sector_count = (root_directory.entries[i].size + 511) / 512;
            for (uint32_t j = 0; j < sector_count; j++) {
                ata_write_sector(start_sector + j, (uint8_t *)(data + j * 512));
            }
            return 0;
        }
    }
    return -1; // 文件不存在
}

// 读取文件内容
int read_file(const char *filename, char *buffer) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_directory.entries[i].is_used &&
            compare_string(root_directory.entries[i].filename, filename) == 0) {
            uint32_t start_sector = root_directory.entries[i].start_sector;
            uint32_t sector_count = (root_directory.entries[i].size + 511) / 512;
            for (uint32_t j = 0; j < sector_count; j++) {
                ata_read_sector(start_sector + j, (uint8_t *)(buffer + j * 512));
            }
            return 0;
        }
    }
    return -1; // 文件不存在
}

// 删除文件
int delete_file(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_directory.entries[i].is_used &&
            compare_string(root_directory.entries[i].filename, filename) == 0) {
            uint32_t start_sector = root_directory.entries[i].start_sector;
            uint32_t sector_count = (root_directory.entries[i].size + 511) / 512;
            free_sectors(start_sector, sector_count);
            root_directory.entries[i].is_used = 0;
            ata_write_sector(3, (uint8_t *)&root_directory);
            return 0;
        }
    }
    return -1; // 文件不存在
}
