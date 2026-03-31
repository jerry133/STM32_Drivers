#ifndef MY_SPI_DRIVER_H
#define MY_SPI_DRIVER_H

#include "stm32h7xx.h" // 引入 H7 暫存器定義

// 狀態列舉
typedef enum {
    MY_SPI_OK       = 0x00U,
    MY_SPI_ERROR    = 0x01U,
    MY_SPI_TIMEOUT  = 0x02U
} My_SPI_Status_t;

// 函式原型
My_SPI_Status_t My_SPI_Init(SPI_TypeDef *SPIx);
My_SPI_Status_t My_SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *pData, uint16_t Size);

#endif /* MY_SPI_DRIVER_H */
