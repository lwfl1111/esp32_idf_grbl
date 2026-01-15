#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

// ESP32 IDF Grbl 配置文件

// ===========================
// 串口配置
// ===========================
#define UART_NUM UART_NUM_0
#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_3)
#define BAUD_RATE 115200
#define UART_DRIVER_BUF_SIZE (256) // UART驱动自带缓冲区大小

// ===========================
// 缓冲区配置
// ===========================
#define RXBUFFERSIZE 256              // 接收缓冲区大小
#define BLOCK_BUFFER_SIZE 16          // 块缓冲区大小
#define SEGMENT_BUFFER_SIZE 16        // 段缓冲区大小
#define MUTEX_WAIT_TIME portMAX_DELAY // 等代互斥锁时间

// ===========================
// 实时指令定义
// ===========================
// 定义实时指令特殊字符，这些字符从串口数据流中直接提取，不传递给Grbl行执行解析器。
// 选择在流GCode程序中不存在且不得存在的字符。可以使用ASCII控制字符（如果用户设置可用）。
// 也可以选择扩展ASCII代码(>127)，这些代码永远不会出现在GCode程序中，可能被接口程序选择。

// 注意：所有覆盖实时命令必须在扩展ASCII字符集中，从字符值128(0x80)开始，到255(0xFF)结束。
typedef enum
{
    CMD_RESET = 0x18, // Ctrl-X
    CMD_STATUS_REPORT = '?',
    CMD_CYCLE_START = '~',
    CMD_FEED_HOLD = '!',
    CMD_SAFETY_DOOR = 0x84,
    CMD_JOG_CANCEL = 0x85,
    CMD_DEBUG_REPORT = 0x86,   // 仅在DEBUG启用时，发送'{}'大括号中的调试报告。
    CMD_FEED_OVR_RESET = 0x90, // 将进给覆盖值恢复到100%。
    CMD_FEED_OVR_COARSE_PLUS = 0x91,
    CMD_FEED_OVR_COARSE_MINUS = 0x92,
    CMD_FEED_OVR_FINE_PLUS = 0x93,
    CMD_FEED_OVR_FINE_MINUS = 0x94,
    CMD_RAPID_OVR_RESET = 0x95, // 将快速移动覆盖值恢复到100%。
    CMD_RAPID_OVR_MEDIUM = 0x96,
    CMD_RAPID_OVR_LOW = 0x97,
    CMD_RAPID_OVR_EXTRA_LOW = 0x98,     // *不支持*
    CMD_SPINDLE_OVR_RESET = 0x99,       // 将主轴覆盖值恢复到100%。
    CMD_SPINDLE_OVR_COARSE_PLUS = 0x9A, // 154
    CMD_SPINDLE_OVR_COARSE_MINUS = 0x9B,
    CMD_SPINDLE_OVR_FINE_PLUS = 0x9C,
    CMD_SPINDLE_OVR_FINE_MINUS = 0x9D,
    CMD_SPINDLE_OVR_STOP = 0x9E,
    CMD_COOLANT_FLOOD_OVR_TOGGLE = 0xA0,
    CMD_COOLANT_MIST_OVR_TOGGLE = 0xA1,
} Cmd;

// ===========================
// 系统配置
// ===========================
#define AXIS_COUNT 3 // X, Y, Z 三轴

// ===========================
// 运动控制配置
// ===========================
#define MINIMUM_JUNCTION_SPEED 0.0      // mm/min
#define MINIMUM_FEED_RATE 1.0           // mm/min
#define N_ARC_CORRECTION 12             // 圆弧生成迭代次数
#define ARC_ANGULAR_TRAVEL_EPSILON 5E-7 // 弧度
#define DWELL_TIME_STEP 50              // 暂停时间步长，单位：毫秒

// ===========================
// 调试配置
// ===========================
#define DEBUG_ENABLED false // 调试模式开关

#endif /* __CONFIG_H__ */