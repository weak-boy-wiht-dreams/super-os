#pragma once

#include <stdbool.h>

static char key_buffer[256];
void init_keyboard();
char get_key();
void set_input_pos(int pos);
void set_letter_at_pos(char* buffer, int pos, char letter);
void handle_backspace(char* buffer, int input_pos);
bool at_buffer_end(char* buffer);