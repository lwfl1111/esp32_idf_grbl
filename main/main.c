/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "inc/serial.h"

void app_main(void)
{
    printf("Serial echo test started. Send messages to receive echo!\n");
    serial_init();

    // 串口回显测试主循环
    uint8_t bytes_read;

    for (;;)
    {
        uint16_t available = serial_available();
        // 检查是否有数据可读
        if (available > 0)
        {
            // 读取数据
            if (serial_read(&bytes_read))
            {
                // 回显数据（发送回去）
                serial_write(&bytes_read, 1);
                // 在控制台打印接收到的数据，方便调试
                printf("\n");

                // printf("%d\n", available);
            }
        }

        // 短暂延时，避免占用过多CPU
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
