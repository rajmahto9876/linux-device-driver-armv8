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

#define GICD_BASE           0x08000000UL
#define GICR_BASE           0x080A0000UL
#define GICD_TYPER          0x0004

#define GICR_ISENABLER0     0x10100
#define GICR_ICENABLER0     0x10180

#define GICR_IPRIORITYR     0x10400

#define GICR_IGROUPR0       0x10080

#define GICR_WAKER          0x00014


#define GICD_CTLR             0x0000
#define GICD_ISENABLER        0x0100
#define GICD_ICENABLER        0x0180
#define GICD_IPRIORITYR       0x0400
#define GICD_ITARGETSR        0x0800
#define GICD_ICFGR            0x0C00

#define UART_IRQ             33

#define GICD_ISENABLER       0x0100
#define GICD_IPRIORITYR      0x0400
#define GICD_ICFGR           0x0C00
#define GICD_IROUTER         0x6000

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

static void gic_set_spi_priority(uint32_t irq, uint8_t priority)
{
    volatile uint8_t *priority_reg;

    priority_reg =
        (volatile uint8_t *)(GICD_BASE + GICD_IPRIORITYR + irq);

    *priority_reg = priority;

    __asm__ volatile("dsb sy");
}

static void gic_route_spi(uint32_t irq)
{
    volatile uint64_t *router;
    router = (volatile uint64_t *)(GICD_BASE + GICD_IROUTER + ((uint64_t)irq * 8));

    /*
     * Affinity 0 = CPU0
     */
    *router = 0;
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
}

void gic_enable_irq(void)
{
    volatile uint32_t *group  = (volatile uint32_t *)(GICR_BASE + GICR_IGROUPR0);
    volatile uint32_t *enable = (volatile uint32_t *)(GICR_BASE + GICR_ISENABLER0);
    volatile uint8_t *priority = (volatile uint8_t *)(GICR_BASE + GICR_IPRIORITYR);

    gic_wake_redistributor();

    /* Timer = Interrupt ID 30 */
    *group |= (1u << 30);
    priority[30] = 0x80;
    *enable = (1u << 30);

/*
    UART_IRQ = 33
    33 / 32 = 1
    33 % 32 = 1, GICD_ISENABLER1 bit 1
*/

    gic_route_spi(33);
    gic_set_spi_priority(33, 0x80);

    volatile uint32_t *enable_uart;
    enable_uart = (volatile uint32_t *)(GICD_BASE + GICD_ISENABLER + ((UART_IRQ / 32) * 4));
    *enable_uart |= (1u << (UART_IRQ % 32));

    volatile uint32_t *igroup = (volatile uint32_t *)(GICD_BASE + 0x0080 + 4 * (UART_IRQ / 32));
    *igroup |= (1u << (UART_IRQ % 32));

    volatile uint32_t *icfgr = (volatile uint32_t *)(GICD_BASE + GICD_ICFGR + 4 * (UART_IRQ / 16));
    *icfgr &= ~(0x3u << ((UART_IRQ % 16) * 2));   // 00 = level-sensitive
    
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");

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