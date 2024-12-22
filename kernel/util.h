#pragma once

#include <stdint.h>
#include <stdbool.h>
#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

void memory_copy(uint8_t *source, uint8_t *dest, uint32_t nbytes);

int string_length(char s[]);

void reverse(char s[]);

void int_to_string(int n, char str[]);
void append(char s[], char n);
bool backspace(char s[]);
//int compare_string(char s1[], char s2[]);
int compare_string(const char s1[], const char s2[]);

int string_to_int(const char *str);
void string_copy(char *dest, const char *src);
void get_user_input(char *buffer);
void test_input();

void print_hex(uint32_t value);