#ifndef __GIC_H_
#define __GIC_H_


#include <stdint.h>

#define GICD_BASE       0x08000000UL
#define GICR_BASE       0x080A0000UL

void gic_init(void);
void gic_enable_timer_irq(void);

uint32_t gic_acknowledge(void);
void gic_end_interrupt(uint32_t irq);


#endif /* __GIC_H_ */