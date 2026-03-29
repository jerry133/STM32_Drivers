#ifndef __MY_I2C_DRIVER_H
#define __MY_I2C_DRIVER_H

#include "stm32h7xx.h"  // 引入 H7 暫存器定義

// 狀態列舉
typedef enum {
    MY_I2C_OK       = 0x00U,
    MY_I2C_ERROR    = 0x01U,
    MY_I2C_BUSY     = 0x02U,
    MY_I2C_TIMEOUT  = 0x03U
} My_I2C_Status_t;

// 控制手柄結構體
typedef struct {
    I2C_TypeDef *Instance;  // 指向硬體暫存器基地址 (例如 I2C1)
    uint32_t Timeout;       // 超時設定值
} My_I2C_Handle_t;

// 外部呼叫的 API
void My_I2C_Init(My_I2C_Handle_t *hi2c, I2C_TypeDef *I2Cx);
My_I2C_Status_t My_I2C_Master_Transmit(My_I2C_Handle_t *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
My_I2C_Status_t My_I2C_Master_Receive(My_I2C_Handle_t *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);

#endif /* __MY_I2C_DRIVER_H */
