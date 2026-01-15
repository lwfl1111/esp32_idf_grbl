// 文件简介：本文件实现 ESP32 串口通信功能，包含双缓冲机制和实时指令处理。

#include "driver/uart.h"
#include "esp_log.h"
#include "serial.h"
#include "config.h"
#include "driver/gpio.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "InputBuffer.h"

// 全局变量
InputBuffer_t rx_buffer;
bool is_initialized = false;
TaskHandle_t serial_task_handle = NULL;

// 处理实时指令
void process_real_time_command(Cmd cmd)
{
    switch (cmd)
    {
    case CMD_RESET:
        ESP_LOGI("SERIAL", "Reset command received");
        // 这里添加实际的重置逻辑
        break;
    case CMD_STATUS_REPORT:
        ESP_LOGI("SERIAL", "Status report command received");
        // 这里添加实际的状态报告逻辑
        break;
    case CMD_CYCLE_START:
        ESP_LOGI("SERIAL", "Cycle start command received");
        // 这里添加实际的循环开始逻辑
        break;
    case CMD_FEED_HOLD:
        ESP_LOGI("SERIAL", "Feed hold command received");
        // 这里添加实际的进给保持逻辑
        break;
    case CMD_SAFETY_DOOR:
        ESP_LOGI("SERIAL", "Safety door command received");
        // 这里添加实际的安全门逻辑
        break;
    case CMD_JOG_CANCEL:
        ESP_LOGI("SERIAL", "Jog cancel command received");
        // 这里添加实际的手动控制取消逻辑
        break;
    case CMD_DEBUG_REPORT:
        ESP_LOGI("SERIAL", "Debug report command received");
        // 这里添加实际的调试报告逻辑
        break;
    case CMD_FEED_OVR_RESET:
        ESP_LOGI("SERIAL", "Feed override reset command received");
        // 这里添加实际的进给覆盖重置逻辑
        break;
    case CMD_FEED_OVR_COARSE_PLUS:
        ESP_LOGI("SERIAL", "Feed override coarse plus command received");
        // 这里添加实际的进给覆盖粗调增加逻辑
        break;
    case CMD_FEED_OVR_COARSE_MINUS:
        ESP_LOGI("SERIAL", "Feed override coarse minus command received");
        // 这里添加实际的进给覆盖粗调减少逻辑
        break;
    case CMD_FEED_OVR_FINE_PLUS:
        ESP_LOGI("SERIAL", "Feed override fine plus command received");
        // 这里添加实际的进给覆盖细调增加逻辑
        break;
    case CMD_FEED_OVR_FINE_MINUS:
        ESP_LOGI("SERIAL", "Feed override fine minus command received");
        // 这里添加实际的进给覆盖细调减少逻辑
        break;
    case CMD_RAPID_OVR_RESET:
        ESP_LOGI("SERIAL", "Rapid override reset command received");
        // 这里添加实际的快速移动覆盖重置逻辑
        break;
    case CMD_RAPID_OVR_MEDIUM:
        ESP_LOGI("SERIAL", "Rapid override medium command received");
        // 这里添加实际的快速移动覆盖中等速度逻辑
        break;
    case CMD_RAPID_OVR_LOW:
        ESP_LOGI("SERIAL", "Rapid override low command received");
        // 这里添加实际的快速移动覆盖低速逻辑
        break;
    case CMD_SPINDLE_OVR_RESET:
        ESP_LOGI("SERIAL", "Spindle override reset command received");
        // 这里添加实际的主轴覆盖重置逻辑
        break;
    case CMD_SPINDLE_OVR_COARSE_PLUS:
        ESP_LOGI("SERIAL", "Spindle override coarse plus command received");
        // 这里添加实际的主轴覆盖粗调增加逻辑
        break;
    case CMD_SPINDLE_OVR_COARSE_MINUS:
        ESP_LOGI("SERIAL", "Spindle override coarse minus command received");
        // 这里添加实际的主轴覆盖粗调减少逻辑
        break;
    case CMD_SPINDLE_OVR_FINE_PLUS:
        ESP_LOGI("SERIAL", "Spindle override fine plus command received");
        // 这里添加实际的主轴覆盖细调增加逻辑
        break;
    case CMD_SPINDLE_OVR_FINE_MINUS:
        ESP_LOGI("SERIAL", "Spindle override fine minus command received");
        // 这里添加实际的主轴覆盖细调减少逻辑
        break;
    case CMD_SPINDLE_OVR_STOP:
        ESP_LOGI("SERIAL", "Spindle override stop command received");
        // 这里添加实际的主轴覆盖停止逻辑
        break;
    case CMD_COOLANT_FLOOD_OVR_TOGGLE:
        ESP_LOGI("SERIAL", "Coolant flood override toggle command received");
        // 这里添加实际的冷却液洪水模式覆盖切换逻辑
        break;
    case CMD_COOLANT_MIST_OVR_TOGGLE:
        ESP_LOGI("SERIAL", "Coolant mist override toggle command received");
        // 这里添加实际的冷却液喷雾模式覆盖切换逻辑
        break;
    default:
        ESP_LOGW("SERIAL", "Unknown real-time command: 0x%02X", cmd);
        break;
    }
}

// 判断是否为实时指令
bool is_real_time_command(uint8_t data)
{
    // 根据Grbl_Esp32的实现方式，判断是否为实时指令
    // 1. 检查是否为扩展ASCII字符集（大于等于0x80）
    if (data >= 0x80)
    {
        return true;
    }
    // 2. 检查是否为常用的实时命令
    return (data == CMD_RESET ||
            data == CMD_STATUS_REPORT ||
            data == CMD_CYCLE_START ||
            data == CMD_FEED_HOLD);
}

// 串口数据处理任务
void serial_task(void *pvParameters)
{
    uint8_t data;
    int len;
    while (1)
    {
        // 从UART驱动缓冲区读取数据
        len = uart_read_bytes(UART_NUM, &data, 1, 0);
        if ((len > 0)) // 如果读出数据
        {
            if (InputBuffer_GetSize(&rx_buffer) < RXBUFFERSIZE) // 且二级缓冲区有可写入空间
            {
                // 判断是否为实时指令
                if (is_real_time_command(data))
                {
                    // 是实时指令，立即处理
                    process_real_time_command((Cmd)data);
                }
                else
                {
                    // 不是实时指令，写入环形缓冲区
                    if (!InputBuffer_Write(&rx_buffer, data))
                    {
                        ESP_LOGE("SERIAL", "write error");
                    }
                }

                // 获取驱动层缓存数据长度
                //  size_t size;
                //  uart_get_buffered_data_len(UART_NUM, &size);
                //  printf("%d\n", size);
            }
            else
            {
                ESP_LOGE("SERIAL", "Input buffer overflow");
            }
        }
        // 让出CPU时间片
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// 串口初始化函数
void serial_init(void)
{
    if (is_initialized)
    {
        return;
    }
    // 配置 UART 参数
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // 安装 UART 驱动，使用双缓冲区
    uart_driver_install(UART_NUM, UART_DRIVER_BUF_SIZE, 0, 0, NULL, 0);
    // 配置 UART 引脚
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // 应用 UART 配置
    uart_param_config(UART_NUM, &uart_config);

    // 初始化输入缓冲区
    if (InputBuffer_Init(&rx_buffer) != pdPASS)
    {
        ESP_LOGE("SERIAL", "Failed to initialize input buffer");
        return;
    }

    // 创建串口处理任务
    xTaskCreatePinnedToCore(serial_task, "serial_task", 2048, NULL, 1, &serial_task_handle, 1);

    is_initialized = true;
    ESP_LOGI("SERIAL", "UART initialized successfully with InputBuffer mechanism");
}

// 从输入缓冲区读取数据
bool serial_read(uint8_t *t_data)
{
    uint8_t data;
    if (!is_initialized || t_data == NULL)
    {
        return false;
    }

    if (InputBuffer_Read(&rx_buffer, &data))
    {
        *t_data = data;
        return true; // 返回true表示读取成功
    }

    return false; // 返回false表示读取失败
}

// 向串口发送数据
bool serial_write(const uint8_t *data, uint16_t length)
{
    if (!is_initialized || data == NULL || length == 0)
    {
        return false;
    }

    int bytes_written = uart_write_bytes(UART_NUM, (const char *)data, length);
    return (bytes_written == length); // 写入的字节数等于请求的字节数表示成功
}

// 获取输入缓冲区中的可用数据量
uint16_t serial_available(void)
{
    if (!is_initialized)
    {
        return 0;
    }
    return InputBuffer_GetSize(&rx_buffer);
}