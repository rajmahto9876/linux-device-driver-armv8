/*
gic_init()
Enable Distributor
Enable CPU Interface
Enable System Register Interface
gic_acknowledge()
gic_end_interrupt()

The CPU supports them (QEMU Cortex-A53 does), but the assembler doesn't 
know their aliases.

| Old               | New              |
| ----------------- | ---------------- |
| `ICC_SRE_EL1`     | `S3_0_C12_C12_5` |
| `ICC_PMR_EL1`     | `S3_0_C4_C6_0`   |
| `ICC_IGRPEN1_EL1` | `S3_0_C12_C12_7` |
| `ICC_IAR1_EL1`    | `S3_0_C12_C12_0` |
| `ICC_EOIR1_EL1`   | `S3_0_C12_C12_1` |

*/

#include "gic.h"

#define GICR_ISENABLER0     0x10100
#define GICR_ICENABLER0     0x10180

#define GICR_IPRIORITYR     0x10400

#define GICR_IGROUPR0       0x10080
#define GICD_CTLR           0x0000

#define GICR_WAKER          0x00014

static inline void gic_wake_redistributor(void)
{
    volatile uint32_t *waker = (volatile uint32_t *)(GICR_BASE + GICR_WAKER);

    *waker &= ~(1u << 1);           /* clear ProcessorSleep */
    while (*waker & (1u << 2)) {}   /* wait for ChildrenAsleep to clear */
}

/* Enable System Register Interface */
static inline void gic_enable_sre(void)
{
    uint64_t val;

    __asm__ volatile(
        "mrs %0, S3_0_C12_C12_5\n"
        : "=r"(val));

    val |= 1;

    __asm__ volatile(
        "msr S3_0_C12_C12_5, %0\n"
        :
        : "r"(val));

    __asm__ volatile("isb");
}

static inline void mmio_write32(uint64_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t mmio_read32(uint64_t addr)
{
    return *(volatile uint32_t *)addr;
}

void gic_enable_timer_irq(void)
{
    volatile uint32_t *group  = (volatile uint32_t *)(GICR_BASE + GICR_IGROUPR0);
    volatile uint32_t *enable = (volatile uint32_t *)(GICR_BASE + GICR_ISENABLER0);
    volatile uint8_t *priority = (volatile uint8_t *)(GICR_BASE + GICR_IPRIORITYR);

    gic_wake_redistributor();

    /* Timer = Interrupt ID 30 */
    *group |= (1u << 30);
    priority[30] = 0x80;
    *enable = (1u << 30);
}

/* Initialize CPU Interface */
void gic_init(void)
{
    gic_enable_sre();

    volatile uint32_t *gicd_ctlr = (volatile uint32_t *)(GICD_BASE + GICD_CTLR);
    *gicd_ctlr = (1u << 0)  /* EnableGrp0 */
               | (1u << 1)  /* EnableGrp1 */
               | (1u << 4); /* ARE        */

    /* Allow all priorities */
    __asm__ volatile(
        "mov x0,#0xff\n"
        "msr S3_0_C4_C6_0,x0\n"
        :::"x0");

    /* Enable Group1 interrupts */
    __asm__ volatile(
        "mov x0,#1\n"
        "msr S3_0_C12_C12_7,x0\n"
        :::"x0");

    __asm__ volatile("isb");
}

/* Read Interrupt ID */
uint32_t gic_acknowledge(void)
{
    uint64_t irq;

    __asm__ volatile(
        "mrs %0, S3_0_C12_C12_0"
        : "=r"(irq));

    return (uint32_t)irq;
}

/* End Of Interrupt */
void gic_end_interrupt(uint32_t irq)
{
    __asm__ volatile(
        "msr S3_0_C12_C12_1,%0"
        :
        : "r"((uint64_t)irq));

    __asm__ volatile("isb");
}