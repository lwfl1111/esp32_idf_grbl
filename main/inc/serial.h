#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>
#include "config.h"
// 函数声明
void serial_init(void);
bool serial_read(uint8_t *data);
bool serial_write(const uint8_t *data, uint16_t length);
uint16_t serial_available(void);

// 实时指令处理函数
bool is_real_time_command(uint8_t data);
void process_real_time_command(Cmd cmd);

#endif
