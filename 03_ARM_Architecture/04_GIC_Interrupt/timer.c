/*
	                +----------------------+
                    |   Generic Timer      |
                    +----------+-----------+
                               |
                          PPI 30
                               |
                               ▼
                  +----------------------+
                  |   GIC Distributor    |
                  +----------+-----------+
                             |
                             ▼
                  +----------------------+
                  |   CPU Interface      |
                  +----------+-----------+
                             |
                             ▼
                      Cortex-A53
                             |
                      Exception Vector
                             |
                      irq_handler()

	Read CNTFRQ_EL0
	Program CNTP_TVAL_EL0
	Enable CNTP_CTL_EL0
	Reload timer
	timer_init()

*/

#include "timer.h"

#define TIMER_ENABLE      (1 << 0)
#define TIMER_IMASK       (1 << 1)

static uint64_t timer_freq = 10;

/* Read CNTFRQ_EL0 */
static inline uint64_t timer_get_frequency(void)
{
    uint64_t freq;

    __asm__ volatile(
        "mrs %0, CNTFRQ_EL0"
        : "=r"(freq));

    return freq;
}

/* Reload timer */
void timer_reload(void)
{
    __asm__ volatile(
        "msr CNTP_TVAL_EL0, %0"
        :
        : "r"(timer_freq));
}

/* Initialize Generic Timer */
void timer_init(void)
{
    uint64_t ctrl;

    /* Read timer frequency */
    timer_freq = timer_get_frequency();

    /* Generate interrupt after 1 second */
    __asm__ volatile(
        "msr CNTP_TVAL_EL0, %0"
        :
        : "r"(timer_freq));

    /*
     * CNTP_CTL_EL0
     * Bit0 = Enable
     * Bit1 = Interrupt Mask
     * Bit2 = ISTATUS (Read Only)
     */

    ctrl = TIMER_ENABLE;

    __asm__ volatile(
        "msr CNTP_CTL_EL0, %0"
        :
        : "r"(ctrl));

    __asm__ volatile("isb");
}