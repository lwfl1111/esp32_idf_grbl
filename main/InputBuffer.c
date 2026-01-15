#include "InputBuffer.h"
#include "esp_log.h"
#include <string.h>

// 初始化输入缓冲区,且创建互斥锁
BaseType_t InputBuffer_Init(InputBuffer_t *buffer)
{
    if (buffer == NULL)
        return pdFAIL;

    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;
    memset(buffer->RX_buffer, 0, sizeof(buffer->RX_buffer));

    /* 创建互斥量 */
    buffer->xMutex = xSemaphoreCreateMutex();
    if (buffer->xMutex == NULL)
    {
        return pdFAIL; // 互斥量创建失败
    }
    return pdPASS;
}

/* 写入数据到输入缓冲区 (线程安全) */
bool InputBuffer_Write(InputBuffer_t *buffer, uint8_t data)
{
    bool ret = false;
    if (buffer == NULL)
        return false;

    /* 获取互斥量 (等待 forever) */
    if (xSemaphoreTake(buffer->xMutex, MUTEX_WAIT_TIME) == pdTRUE)
    {
        /* 进入临界区 - 只有拿到互斥量的任务才能执行下面的代码 */
        if (buffer->size < RXBUFFERSIZE)
        {
            buffer->RX_buffer[buffer->head] = data;
            buffer->head = (buffer->head + 1) % RXBUFFERSIZE;
            buffer->size++;
            ret = true;
        }
        /* 离开临界区 - 释放互斥量 */
        xSemaphoreGive(buffer->xMutex);
    }
    return ret;
}

/* 写入多个数据到输入缓冲区 (线程安全) */
/*
 * @brief 写入多个数据到输入缓冲区 (线程安全)
 *
 * @param buffer 输入缓冲区指针
 * @param data 要写入的数据指针
 * @return true 写入成功
 * @return false 写入失败
 */
bool InputBuffer_Write_string(InputBuffer_t *buffer, uint8_t *data)
{
    bool ret = false;
    int data_size = 0;

    if (buffer == NULL || data == NULL)
        return false;

    data_size = strlen((char *)data);
    /* 获取互斥量 (等待 forever) */
    if (xSemaphoreTake(buffer->xMutex, MUTEX_WAIT_TIME) == pdTRUE)
    {
        /* 进入临界区 - 只有拿到互斥量的任务才能执行下面的代码 */
        if ((buffer->size + data_size) < RXBUFFERSIZE) // 还有足够的空间写入
        {
            for (int i = 0; i < data_size; i++)
            {
                buffer->RX_buffer[buffer->head] = data[i];
                buffer->head = (buffer->head + 1) % RXBUFFERSIZE;
            }
            buffer->size += data_size;
            ret = true;
        }
        /* 离开临界区 - 释放互斥量 */
        xSemaphoreGive(buffer->xMutex);
    }
    return ret;
}

/* 从输入缓冲区读取数据 (线程安全) */
// 参数 buffer 输入缓冲区指针
// 参数 data 用于存储读取数据的指针
// 返回值 true 读取成功, false 读取失败
bool InputBuffer_Read(InputBuffer_t *buffer, uint8_t *data)
{
    bool ret = false;

    if (buffer == NULL || data == NULL)
        return false;

    /* 获取互斥量 (等待 forever) */
    if (xSemaphoreTake(buffer->xMutex, MUTEX_WAIT_TIME) == pdTRUE)
    {
        /* 进入临界区 */
        if (buffer->size > 0)
        {
            *data = buffer->RX_buffer[buffer->tail];
            buffer->tail = (buffer->tail + 1) % RXBUFFERSIZE;
            buffer->size--;
            ret = true;
        }
        /* 离开临界区 */
        xSemaphoreGive(buffer->xMutex);
    }
    return ret;
}

/* 获取输入缓冲区当前数据大小*/
uint16_t InputBuffer_GetSize(InputBuffer_t *buffer)
{
    uint16_t size = 0;

    if (buffer == NULL)
        return 0;

    size = buffer->size;
    return size;
}

/* 清空输入缓冲区 (线程安全) */
void InputBuffer_Clear(InputBuffer_t *buffer)
{
    if (buffer == NULL)
        return;

    /* 获取互斥量 (等待 forever) */
    if (xSemaphoreTake(buffer->xMutex, MUTEX_WAIT_TIME) == pdTRUE)
    {
        buffer->head = 0;
        buffer->tail = 0;
        buffer->size = 0;
        memset(buffer->RX_buffer, 0, sizeof(buffer->RX_buffer));
        xSemaphoreGive(buffer->xMutex);
    }
}
