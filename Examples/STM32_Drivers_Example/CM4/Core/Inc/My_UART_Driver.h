#ifndef MY_UART_DRIVER_H
#define MY_UART_DRIVER_H

#include "stm32h7xx.h" // 確保引入 H7 的 CMSIS 定義檔

// 狀態列舉，保持跟 I2C 類似的良好習慣
typedef enum {
    MY_UART_OK       = 0x00U,
    MY_UART_ERROR    = 0x01U,
    MY_UART_TIMEOUT  = 0x02U
} My_UART_Status_t;

// 函式原型宣告
My_UART_Status_t My_UART_Init(USART_TypeDef *USARTx, uint32_t PeriphClock, uint32_t BaudRate);
My_UART_Status_t My_UART_Transmit(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);

#endif /* MY_UART_DRIVER_H */
