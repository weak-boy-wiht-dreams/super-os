#pragma once

#include "../kernel/util.h"

#define PIT_FREQUENCY 1193180   // PIT芯片的输入频率 (这个神奇的数据是咋来的)
#define TIMER_FREQ 100

void init_timer(uint32_t freq);
void delay(int milliseconds);