/* 
    The purpose of this file is to include the actual tx_port.h file for current cpu architecture
    Author: Neo Xiong <xiongyu0523@gmail.com> 
*/

#ifndef TX_PORT_H
#define TX_PORT_H

#if defined(ARDUINO_ARCH_SAMD)
#if defined(__SAMD21__)
#include "../ports/arch/cortex_m0/tx_port_cm0.h"
#elif defined(__SAMD51__)
#include "../ports/arch/cortex_m4/tx_port_cm4.h"
#else
#error "Microcontroller family is not supported!"
#endif
#endif

#endif



