# I2C Master Register-Level Driver

This driver provides a set of core API functions to control the I2C peripherals of the **STM32H755** by directly manipulating hardware registers, bypassing the HAL layer for maximum efficiency.

## Hardware Abstraction & Peripheral Mapping

Instead of using the HAL library, this driver maps the `I2C_TypeDef` structure directly to the I2C1 base address (`0x40005400`).

### Register Usage Breakdown

| Register | Purpose in this Driver | Key Bit Operations |
| :--- | :--- | :--- |
| **CR1** | Control Register 1 | `PE` (Peripheral Enable) to power on the I2C block. |
| **CR2** | Control Register 2 | `SADD` (Slave Address), `NBYTES` (Data Length), `START` (Generate Start condition), and `AUTOEND` (Automatic Stop). |
| **ISR** | Interrupt & Status | Monitoring `TXIS` (Transmit Ready), `RXNE` (Receive Ready), and `STOPF` (Stop Detection). |
| **TXDR** | Transmit Data | Direct 8-bit data injection for SCL/SDA shifting. |
| **RXDR** | Receive Data | Direct 8-bit data extraction from the internal buffer. |
| **ICR** | Interrupt Clear | `STOPCF` to clear the Stop detection flag manually. |

---

## API Reference & Usage

### 1. I2C Module Initialization
This function must be called before any communication tasks. It binds the hardware instance to your handle and enables the peripheral power.

* **Function Prototype**:
    ```c
    My_I2C_Status_t My_I2C_Init(My_I2C_Handle_t *hi2c, I2C_TypeDef *I2Cx);
    ```
* **Input Parameters**:
    1. `*hi2c`: Pointer to the custom control structure (used for timeout and state management).
    2. `*I2Cx`: Base address pointer of the hardware register (e.g., `I2C1`, `I2C2`).
* **Example**:
    ```c
    My_I2C_Handle_t hmy_i2c1;
    hmy_i2c1.Timeout = 100000;
    My_I2C_Init(&hmy_i2c1, I2C1); // Initialize I2C1 and bind it to the handle
    ```

---

### 2. Master Transmit
Sends commands or data strings to a slave device (e.g., SHT31 sensor).

* **Function Prototype**:
    ```c
    My_I2C_Status_t My_I2C_Master_Transmit(My_I2C_Handle_t *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    ```
* **Input Parameters**:
    1. `hi2c`: Pointer to the initialized driver handle.
    2. `DevAddress`: 7-bit target device address. **Note: Must be shifted left by 1 bit** (e.g., `0x44 << 1`).
    3. `pData`: Pointer to the data array to be sent.
    4. `Size`: Number of bytes to transmit.
* **Example**:
    ```c
    uint8_t cmd[] = {0x24, 0x00}; // SHT31 Measurement Command
    My_I2C_Master_Transmit(&hmy_i2c1, (0x44 << 1), cmd, 2);
    ```

---

### 3. Master Receive
Reads data frames returned by the slave device.

* **Function Prototype**:
    ```c
    My_I2C_Status_t My_I2C_Master_Receive(My_I2C_Handle_t *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    ```
* **Input Parameters**:
    1. `hi2c`: Pointer to the initialized driver handle.
    2. `DevAddress`: 7-bit target device address (shifted left by 1 bit).
    3. `pData`: **Pointer to the reception buffer** (received data will be stored here).
    4. `Size`: Number of bytes expected to receive (e.g., 6 bytes for SHT31).
* **Example**:
    ```c
    uint8_t rx_buf[6];
    My_I2C_Master_Receive(&hmy_i2c1, (0x44 << 1), rx_buf, 6);
    ```

---
