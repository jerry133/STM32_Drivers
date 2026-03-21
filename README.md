# STM32 Driver Library

A dedicated driver library developed for **STM32 series microcontrollers**. This project bypasses the heavy abstraction of the STM32 HAL (Hardware Abstraction Layer), opting instead for **Direct Register-Level Manipulation** to achieve high-efficiency, low-latency, and precise hardware control.

## Core Philosophy
* Eliminates redundant HAL overhead and checks, reducing instruction cycles.
* Direct bit-level operations for absolute mastery over hardware state machines.
* Focuses on understanding the ARM Cortex-M peripheral architecture and register mapping.

---

## Driver Status

This library is continuously expanding. Click the links below to view detailed wiring diagrams and code examples for each module:

| Module | Status | Target Device / Application | Documentation |
| :--- | :--- | :--- | :--- |
| **I2C Master** | ✅ Completed | SHT31 Temp/Humi Sensor | [View Details](./I2C/README.md) |
| **UART / USART** | In Progress | Debug Console (USART3) | [Coming Soon] |
| **SPI Master** | In Progress | ST7735 Display Control | [Coming Soon] |

---

## Development Environment

* **Hardware**: STM32H755 (Dual-Core) / NUCLEO-H755ZI-Q
* **IDE**: STM32CubeIDE
* **Language**: Embedded C
