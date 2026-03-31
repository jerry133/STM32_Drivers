#include "My_SPI_Driver.h"

/**
 * @brief  初始化 SPI 為主機發送模式 (ST7735 螢幕專用)
 * @param  SPIx: 硬體實例 (例如 SPI1)
 */
My_SPI_Status_t My_SPI_Init(SPI_TypeDef *SPIx) {

    // 1. 安全第一：設定前必須先關閉 SPI (清除 SPE 位元)
    SPIx->CR1 &= ~SPI_CR1_SPE;

    // 2. 配置 CFG1 暫存器 (速度與資料寬度)
    //    - MBR (Master Baud Rate): 設為 001 (除以 4)。75MHz / 4 = 18.75MHz
    //    - DSIZE (Data Size): STM32H7 的定義是 (位元數 - 1)，所以 8-bit 要填入 7
    SPIx->CFG1 = (1UL << SPI_CFG1_MBR_Pos) |    // Baud rate = SPI_CLK / 4
                 (7UL << SPI_CFG1_DSIZE_Pos);   // Data size = 8 bits

    // 3. 配置 CFG2 暫存器 (通訊協定與身分)
    //    - MASTER: 設為主機模式
    //    - SSM: 開啟軟體 NSS 管理 (因為我們會用 GPIO 手動控制 CS 腳位)
    //    - CPOL = 0, CPHA = 0 (預設就是 0，符合 ST7735 的 Mode 0 時序)
    //SPIx->CFG2 = SPI_CFG2_MASTER | SPI_CFG2_SSM;
    //並加上 COMM = 01 (Simplex Transmitter)
    SPIx->CFG2 = SPI_CFG2_MASTER | SPI_CFG2_SSM | (1UL << SPI_CFG2_COMM_Pos);
    // 4. 配置 CR1 暫存器 (軟體 CS 腳位內部電位)
    //    - SSI: 配合 SSM=1，將內部 NSS 訊號拉高，避免觸發 Master Mode Fault
    SPIx->CR1 |= SPI_CR1_SSI;

    // 5. 設定完成，啟動 SPI 總電源
    SPIx->CR1 |= SPI_CR1_SPE;

    return MY_SPI_OK;
}

/**
 * @brief  透過 SPI 發送資料陣列 (STM32H7 封包模式)
 * @param  SPIx: 硬體實例
 * @param  pData: 欲發送的資料緩衝區指標
 * @param  Size: 發送位元組數量 (ST7735 刷屏時數量會很大)
 */
/*My_SPI_Status_t My_SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *pData, uint16_t Size) {

    // 1. 告訴硬體這次要傳幾顆子彈 (TSIZE)
    SPIx->CR2 = Size;

    // 2. 按下發射按鈕 (CSTART: Start transfer)
    SPIx->CR1 |= SPI_CR1_CSTART;

    // 3. 開始瘋狂填裝子彈 (迴圈塞資料)
    for (uint16_t i = 0; i < Size; i++) {

        // 3a. 等待發送 FIFO 空間足夠 (TXP: Tx-Packet space available)
        while (!(SPIx->SR & SPI_SR_TXP)) {
            // Wait...
        }

        // 3b. 將資料寫入 TXDR
        // [極度重要] 在 H7 中，如果你的 DSIZE 是 8-bit，你必須用 8-bit 的指標去寫 TXDR！
        // 如果你直接寫 SPIx->TXDR = pData[i]，編譯器會當作 32-bit 寫入，導致 FIFO 裝填錯亂。
        *(__IO uint8_t *)&SPIx->TXDR = pData[i];
    }

    // 4. 所有子彈填裝完畢，等待整批傳輸徹底結束 (EOT: End of Transfer)
    // 這確保最後一個 bit 已經完全離開 SPI 實體腳位
    while (!(SPIx->SR & SPI_SR_EOT)) {
        // Wait...
    }

    // 5. 清除 EOT 旗標與 TXTF (Tx Transfer Filled) 旗標
    // H7 規定必須透過 IFCR (Interrupt/Flag Clear Register) 寫 1 來清除狀態
    SPIx->IFCR |= SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    return MY_SPI_OK;
}*/
My_SPI_Status_t My_SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *pData, uint16_t Size) {

    // 如果長度為 0，直接返回，避免硬體卡死
    if (Size == 0) return MY_SPI_OK;

    // 0. 發送前，先清除殘留的結束旗標 (注意：IFCR 是 Write 1 to clear，必須用 = )
    SPIx->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    // 1. 告訴硬體這次要傳幾個 Byte (TSIZE)
    SPIx->CR2 = Size;

    // 2. 按下發射按鈕 (CSTART)
    SPIx->CR1 |= SPI_CR1_CSTART;

    // 3. 迴圈塞資料
    for (uint16_t i = 0; i < Size; i++) {
        // 等待 FIFO 有空間
        while (!(SPIx->SR & SPI_SR_TXP)) {}

        // 寫入 8-bit 資料
        *(__IO uint8_t *)&SPIx->TXDR = pData[i];
    }

    // 4. 等待整批傳輸徹底結束
    while (!(SPIx->SR & SPI_SR_EOT)) {}

    // 5. 再次清除旗標，為下一次通訊做準備
    SPIx->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    return MY_SPI_OK;
}
