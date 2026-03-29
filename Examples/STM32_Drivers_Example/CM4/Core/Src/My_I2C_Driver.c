#include "My_I2C_Driver.h"

// 簡單的延遲計數，用來做超時保護
#define I2C_TIMEOUT_MAX 0x10000

void My_I2C_Init(My_I2C_Handle_t *hi2c, I2C_TypeDef *I2Cx) {
    hi2c->Instance = I2Cx;
    hi2c->Timeout = I2C_TIMEOUT_MAX;

    // 備註：硬體腳位 (PB8, PB9) 與時鐘 (RCC) 我們暫時沿用 CubeMX 產生的 HAL_I2C_MspInit 來處理。
    // 這裡只做最基本的周邊啟動。
    hi2c->Instance->CR1 |= I2C_CR1_PE; // 啟用 I2C (Peripheral Enable)
}

My_I2C_Status_t My_I2C_Master_Transmit(My_I2C_Handle_t *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
    I2C_TypeDef *I2Cx = hi2c->Instance;
    uint32_t timeout_cnt;

    // 1. 設定 CR2 暫存器 (目標位址, 傳輸大小, START 訊號, 自動產生 STOP)
    // 注意：發送模式下，不用設置 RD_WRN (預設為 0，代表 Write)
    // (DevAddress & 0x3FF) -> 11 1111 1111 雖然一般的 I2C 地址只有 7 位元，但 STM32 支援 10 位元地址。做 & 0x3FF 是為了確保你傳進來的地址不會超出 10 位元的範圍
    I2Cx->CR2 = (uint32_t)((DevAddress & 0x3FF) |
                           (Size << I2C_CR2_NBYTES_Pos) |
                           I2C_CR2_AUTOEND |
                           I2C_CR2_START);

    // 2. 迴圈發送數據
    for (uint16_t i = 0; i < Size; i++) {
        timeout_cnt = hi2c->Timeout;

        // 等待 TXIS (發送緩衝區為空)
        while (!(I2Cx->ISR & I2C_ISR_TXIS)) {
            if (--timeout_cnt == 0) return MY_I2C_TIMEOUT;
        }

        // 將資料寫入 TXDR 暫存器
        I2Cx->TXDR = pData[i];
    }

    // 3. 等待傳輸完成 (STOPF 旗標)
    timeout_cnt = hi2c->Timeout;
    while (!(I2Cx->ISR & I2C_ISR_STOPF)) {
        if (--timeout_cnt == 0) return MY_I2C_TIMEOUT;
    }

    // 4. 清除 STOP 旗標
    I2Cx->ICR |= I2C_ICR_STOPCF;

    // 5. 清除 CR2 配置 (歸零)
    I2Cx->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND);

    return MY_I2C_OK;
}

My_I2C_Status_t My_I2C_Master_Receive(My_I2C_Handle_t *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
    I2C_TypeDef *I2Cx = hi2c->Instance;
    uint32_t timeout_cnt;

    // 1. 設定 CR2 暫存器 (目標位址, 傳輸大小, START 訊號, 自動產生 STOP)
    // 注意：接收模式下，必須設置 RD_WRN (Read/Write 設為 1，代表 Read)
    I2Cx->CR2 = (uint32_t)((DevAddress & 0x3FF) |
                           (Size << I2C_CR2_NBYTES_Pos) |
                           I2C_CR2_AUTOEND |
                           I2C_CR2_RD_WRN |
                           I2C_CR2_START);

    // 2. 迴圈接收數據
    for (uint16_t i = 0; i < Size; i++) {
        timeout_cnt = hi2c->Timeout;

        // 等待 RXNE (接收緩衝區非空)
        while (!(I2Cx->ISR & I2C_ISR_RXNE)) {
            if (--timeout_cnt == 0) return MY_I2C_TIMEOUT;
        }

        // 從 RXDR 暫存器讀取資料
        pData[i] = I2Cx->RXDR;
    }

    // 3. 等待通訊結束 (STOPF 旗標)
    timeout_cnt = hi2c->Timeout;
    while (!(I2Cx->ISR & I2C_ISR_STOPF)) {
        if (--timeout_cnt == 0) return MY_I2C_TIMEOUT;
    }

    // 4. 清除 STOP 旗標
    I2Cx->ICR |= I2C_ICR_STOPCF;

    // 5. 清除 CR2 配置 (歸零)
    I2Cx->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);

    return MY_I2C_OK;
}
