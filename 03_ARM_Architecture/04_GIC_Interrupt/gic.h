#ifndef __GIC_H_
#define __GIC_H_


#include <stdint.h>

void gic_init(void);
void gic_enable_irq(void);

uint32_t gic_acknowledge(void);
void gic_end_interrupt(uint32_t irq);


#endif /* __GIC_H_ */