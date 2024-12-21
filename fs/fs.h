#pragma once
#include <stdint.h>
#include "../drivers/display.h"
#include "../drivers/ports.h"
#include "../kernel/util.h"

#define MAX_FILENAME 32
#define MAX_FILES 64
#define MAX_SECTORS 1024

typedef struct {
    char filename[MAX_FILENAME];
    uint32_t size;
    uint32_t start_sector;
    uint32_t sector_count;
    uint8_t is_directory;
    uint8_t is_used;
} file_entry_t;

 typedef struct {
    file_entry_t entries[MAX_FILES];
} directory_t;

extern directory_t root_directory;

void ata_read_sector(uint32_t lba, uint8_t *buffer);
void ata_write_sector(uint32_t lba, uint8_t *buffer);

void init_root_directory();
int create_directory(const char *dirname);
void list_directory();

int create_file(const char *filename, uint32_t size);
int write_file(const char *filename, const char *data);
int read_file(const char *filename, char *buffer);
uint32_t allocate_sectors(uint32_t count);
void free_sectors(uint32_t start, uint32_t count);
void init_sector_bitmap();
int delete_file(const char *filename);

void ata_identify();