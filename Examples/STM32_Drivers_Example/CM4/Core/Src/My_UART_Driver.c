#include "My_UART_Driver.h"

/**
 * @brief  初始化 USART 發送功能 (8N1: 8資料位, 無校驗, 1停止位, 16倍過取樣)
 * @param  USARTx: 硬體實例 (如 USART3)
 * @param  PeriphClock: 該 USART 掛載的時脈頻率 (如 32000000)
 * @param  BaudRate: 目標鮑率 (如 115200)
 */
My_UART_Status_t My_UART_Init(USART_TypeDef *USARTx, uint32_t PeriphClock, uint32_t BaudRate) {

    // 1. 設定前先關閉 USART (清除 UE 位元)
    USARTx->CR1 &= ~USART_CR1_UE;

    // 2. 設定傳輸速度 (Baud Rate)
    // 公式: BRR = USART_Clock / Baud_Rate (預設 16 倍過取樣)
    USARTx->BRR = (uint32_t)(PeriphClock / BaudRate);

    // 3. 基本格式設定：因為硬體預設就是 8-bit, Parity None, 1 Stop bit, OVER8=0，
    // 所以我們甚至不用動 CR2 和 CR3，直接在 CR1 開啟發送器 (TE) 即可。
    USARTx->CR1 |= USART_CR1_TE;

    // 4. 開啟 USART 總電源 (UE)
    USARTx->CR1 |= USART_CR1_UE;

    return MY_UART_OK;
}

/**
 * @brief  透過 USART 發送資料陣列 (輪詢模式)
 * @param  USARTx: 硬體實例
 * @param  pData: 欲發送的資料緩衝區指標
 * @param  Size: 發送位元組數量
 */
My_UART_Status_t My_UART_Transmit(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size) {

    // 逐一發送每一個 Byte
    for (uint16_t i = 0; i < Size; i++) {

        // 1. 等待發送資料暫存器為空 (TXE_TXFNF: Transmit Data Register Empty)
        // STM32H7 引入了 FIFO 機制，所以 TXE 的巨集名稱多加了 TXFNF (TX FIFO Not Full)
        while (!(USARTx->ISR & USART_ISR_TXE_TXFNF)) {
            // 可以在此加入 Timeout 遞減
        }

        // 2. 將資料寫入發送暫存器 (TDR)
        USARTx->TDR = pData[i];
    }

    // 3. 所有資料塞入後，等待最後一個位元真正在實體線路上傳輸完畢 (TC: Transmission Complete)
    while (!(USARTx->ISR & USART_ISR_TC)) {
        // 等待硬體將最後一滴資料推出去
    }

    return MY_UART_OK;
}
