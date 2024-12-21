#include "fs.h"  // 包含文件系统相关的全局定义和函数声明
#include "../kernel/util.h"
// 定义根目录表
directory_t root_directory;

// 初始化根目录表
void init_root_directory() {
    ata_read_sector(3, (uint8_t *)&root_directory); // 从磁盘扇区 3 加载根目录表

    // 检查根目录表是否已初始化
    if (root_directory.entries[0].is_used == 0xFF) { // 如果未初始化
        for (int i = 0; i < MAX_FILES; i++) {
            root_directory.entries[i].is_used = 0; // 将所有条目标记为空闲
        }
        ata_write_sector(3, (uint8_t *)&root_directory); // 写回根目录表到磁盘
    }
}

// 创建一个新目录
int create_directory(const char *dirname) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!root_directory.entries[i].is_used) { // 找到空闲条目
            root_directory.entries[i].is_used = 1; // 标记为已使用
            string_copy(root_directory.entries[i].filename, dirname); // 设置目录名
            root_directory.entries[i].is_directory = 1; // 标记为目录
            root_directory.entries[i].start_sector = allocate_sectors(1); // 分配一个扇区
            ata_write_sector(3, (uint8_t *)&root_directory); // 写回根目录表
            return 0; // 成功创建
        }
    }
    return -1; // 无法创建目录
}

// 列出根目录内容
void list_directory() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (root_directory.entries[i].is_used) { // 条目已使用
            print_string(root_directory.entries[i].filename); // 打印文件/目录名
            if (root_directory.entries[i].is_directory) {
                print_string(" <DIR>"); // 如果是目录，标记为 <DIR>
            }
            print_nl(); // 换行
        }
    }
}
