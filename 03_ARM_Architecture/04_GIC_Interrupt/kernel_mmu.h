#ifndef __KERNEL_MMU_H_
#define __KERNEL_MMU_H_

#include <stdint.h>

#define PAGE_SIZE           4096
#define PAGE_TABLE_ENTRIES   512

/*
+-------------------------------------------------------------+
| Next Level Table Address                          | Type |V |
+-------------------------------------------------------------+
63                                                  2  1    0

For a Table Descriptor:
| Bit | Meaning | Value |
| --- | ------- | ----: |
| 0   | Valid   |     1 |
| 1   | Table   |     1 |

*/

/*
	KERNELMEMORY 
	KERNEL_ATTRIBUTES
*/
#define VIRTUAL_ADDRESS 		0x40000000
#define PHYSICAL_ADDRESS		VIRTUAL_ADDRESS
#define KERNEL_SIZE				0x00200000

#define NORMAL_MEMORY ( PTE_VALID | \
						PTE_PAGE | \
						PTE_AF   | \
						PTE_SH_INNER | \
						PTE_ATTRINDX(0) \
					)

/* Descriptor bits */
#define PTE_VALID              (1UL << 0)
#define PTE_TABLE              (1UL << 1)
#define PTE_PAGE               (1UL << 1)

#define MAIR_ATTR_NORMAL   0xFFUL
#define MAIR_ATTR_DEVICE   0x04UL

#define MAIR_VALUE 	((MAIR_ATTR_NORMAL << 0) | (MAIR_ATTR_DEVICE << 8))

#define TCR_T0SZ          (16UL)
#define TCR_IRGN0_WBWA    (1UL << 8)
#define TCR_ORGN0_WBWA    (1UL << 10)
#define TCR_SH0_INNER     (3UL << 12)
#define TCR_TG0_4K        (0UL << 14)
#define TCR_IPS_40BIT     (2UL << 32)

#define TCR_VALUE \
    (TCR_T0SZ |        \
     TCR_IRGN0_WBWA |  \
     TCR_ORGN0_WBWA |  \
     TCR_SH0_INNER |   \
     TCR_TG0_4K |      \
     TCR_IPS_40BIT)

/* Memory attributes */
#define PTE_ATTRINDX(x)        ((uint64_t)(x) << 2)

/* Shareability */
#define PTE_SH_INNER           (3UL << 8)

/* Access Flag */
#define PTE_AF                 (1UL << 10)

#define VALIDATE_TABLE_DESCRIPTOR_VALUE (0x03)
#define VALIDATE_TABLE_DESCRIPTOR(x)	((x) = (x) | VALIDATE_TABLE_DESCRIPTOR_VALUE)

#define TABLE_DESC(addr) (((uint64_t)(addr)) | PTE_VALID | PTE_TABLE)
#define PAGE_DESC(addr, attr) (((uint64_t)(addr)) | (attr))

/* Virtual Address Index Macros */
#define PT_INDEX_MASK 				0x1FFUL
#define L0_INDEX(va) (((va) >> 39) & PT_INDEX_MASK)
#define L1_INDEX(va) (((va) >> 30) & PT_INDEX_MASK)
#define L2_INDEX(va) (((va) >> 21) & PT_INDEX_MASK)
#define L3_INDEX(va) (((va) >> 12) & PT_INDEX_MASK)

extern uint64_t l0_table[];
extern uint64_t l1_table[];
extern uint64_t l2_table[];
extern uint64_t l3_table[];

void mmu_init(void);
void build_translation_tables(void);
#endif /* __KERNEL_MMU_H_ */