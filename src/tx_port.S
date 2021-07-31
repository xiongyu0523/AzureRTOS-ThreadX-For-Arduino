#if defined(ARDUINO_ARCH_SAMD)
#if defined(__SAMD21G18A__)
#include "../ports/bsp/samd21/tx_initialize_low_level.S"
#include "../ports/arch/cortex_m0/tx_thread_context_restore.S"
#include "../ports/arch/cortex_m0/tx_thread_context_save.S"
#include "../ports/arch/cortex_m0/tx_thread_interrupt_control.S"
#include "../ports/arch/cortex_m0/tx_thread_interrupt_disable.S"
#include "../ports/arch/cortex_m0/tx_thread_interrupt_restore.S"
#include "../ports/arch/cortex_m0/tx_thread_schedule.S"
#include "../ports/arch/cortex_m0/tx_thread_stack_build.S"
#include "../ports/arch/cortex_m0/tx_thread_system_return.S"
#include "../ports/arch/cortex_m0/tx_timer_interrupt.S"
#elif defined(__SAMD51__)
#include "../ports/bsp/samd51/tx_initialize_low_level.S"
#include "../ports/arch/cortex_m0/tx_thread_context_restore.S"
#include "../ports/arch/cortex_m0/tx_thread_context_save.S"
#include "../ports/arch/cortex_m0/tx_thread_interrupt_control.S"
#include "../ports/arch/cortex_m0/tx_thread_interrupt_disable.S"
#include "../ports/arch/cortex_m0/tx_thread_interrupt_restore.S"
#include "../ports/arch/cortex_m0/tx_thread_schedule.S"
#include "../ports/arch/cortex_m0/tx_thread_stack_build.S"
#include "../ports/arch/cortex_m0/tx_thread_system_return.S"
#include "../ports/arch/cortex_m0/tx_timer_interrupt.S"
#else
#error "Microcontroller family is not supported!"
#endif
#endif
