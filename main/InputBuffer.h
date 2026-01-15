#ifndef __INPUTBUFFER_H__
#define __INPUTBUFFER_H__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h> // 提供互斥量 API
#include "config.h"

/* 环形缓冲区结构体 */
typedef struct
{
    uint8_t RX_buffer[RXBUFFERSIZE]; // 数据存储数组
    uint16_t head;                   // 写指针（下一个要写入的位置）
    uint16_t tail;                   // 读指针（下一个要读取的位置）
    uint16_t size;                   // 当前缓冲区中数据的大小
    SemaphoreHandle_t xMutex;        // FreeRTOS 互斥量句柄
} InputBuffer_t;

// 初始化输入缓冲区,且创建互斥锁
BaseType_t InputBuffer_Init(InputBuffer_t *buffer);
// 写入数据到输入缓冲区
bool InputBuffer_Write(InputBuffer_t *buffer, uint8_t data);
// 写入多个数据到输入缓冲区 注意只能是字符串，因为非字符串不以/0结尾
bool InputBuffer_Write_string(InputBuffer_t *buffer, uint8_t *data);
// 从输入缓冲区读取数据
bool InputBuffer_Read(InputBuffer_t *buffer, uint8_t *data);
// 获取输入缓冲区当前数据大小
uint16_t InputBuffer_GetSize(InputBuffer_t *buffer);
// 清空输入缓冲区
void InputBuffer_Clear(InputBuffer_t *buffer);

#endif