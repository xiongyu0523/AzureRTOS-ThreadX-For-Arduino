# Azure RTOS ThreadX For Arduino

[![Arduino_CI](https://github.com/xiongyu0523/AzureRTOS-ThreadX-For-Arduino/workflows/Arduino_CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)

This is a port of Azure RTOS ThreadX to Arduino as a Library. For more information about Azure RTOS, please visit Microsoft Doc and source code on Github.

## Hardware support

The port and provided demo is verified on following board and Arduino Core. 

| Board | Chip | Architecture | Verified Arduino Core | 
| - | - | - | -|
| [Seeeduino XIAO](https://wiki.seeedstudio.com/Seeeduino-XIAO/) | [ATSAMD21](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/32-bit-mcus/sam-32-bit-mcus/sam-d) | Cortex-M0+ | [Seeed-Studio/ArduinoCore-samd 1.8.2](https://github.com/Seeed-Studio/ArduinoCore-samd)
| [Seeeduino Wio Terminal](https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/) | [ATSAMD51](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/32-bit-mcus/sam-32-bit-mcus/sam-d) | Cortex-M4 | [Seeed-Studio/ArduinoCore-samd 1.8.2](https://github.com/Seeed-Studio/ArduinoCore-samd)
| [B-L4S5I-IOT01A](https://www.st.com/en/evaluation-tools/b-l4s5i-iot01a.html) | [STM32L4S5](https://www.st.com/zh/microcontrollers-microprocessors/stm32l4r5-s5.html) | Cortex-M4 | [stm32duino/Arduino_Core_STM32 2.0.0](https://github.com/stm32duino/Arduino_Core_STM32)
| [32F746GDISCOVERY](https://www.st.com/en/evaluation-tools/32f746gdiscovery.html) | [STM32F746](https://www.st.com/en/microcontrollers-microprocessors/stm32f7x6.html) | Cortex-M7 | [stm32duino/Arduino_Core_STM32 2.0.0](https://github.com/stm32duino/Arduino_Core_STM32)

## Version

The version of this library is evolving independent to Azure RTOS ThreadX version. Here is a tracking table:

| Library version | ThreadX version | Note |
| - | - | - |
| v1.0.0 | [v6.1.7](https://github.com/azure-rtos/threadx/tree/v6.1.7_rel)  | Initial release |

## License

This repository inherit Azure RTOS license from Microsoft. See [LICENSE.txt](./LICENSE.txt) and [LICENSED-HARDWARE.txt](./LICENSED-HARDWARE.txt).
