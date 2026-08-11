#include "kernel_mmu.h"

/* Because each ARM64 Page Table Entry is 8 bytes i.e., 64 bits */
uint64_t l0_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint64_t l1_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint64_t l2_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint64_t l3_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

/* 
    _Static_assert is a keyword introduced in the C11 standard that performs 
    compile-time assertions, ensuring that specified conditions 
    are met before the program runs
*/

_Static_assert(sizeof(l0_table) == 4096, "L0 table wrong size");
_Static_assert(sizeof(l1_table) == 4096, "L1 table wrong size");
_Static_assert(sizeof(l2_table) == 4096, "L2 table wrong size");
_Static_assert(sizeof(l3_table) == 4096, "L3 table wrong size");

/*
    L0/L1/L2, is Table Descriptor
    L3 entry is a Page Descriptor as it is actual mapping of the physical page mapping
*/
static inline uint64_t make_table_desc(void *table)
{
    return ((uint64_t)table) | PTE_VALID | PTE_TABLE;
}

static inline uint64_t make_page_desc(uint64_t pa, uint64_t attr)
{
    return pa | attr;
}

static void ensure_page_tables(uint64_t va)
{
    uint64_t l0 = L0_INDEX(va);
    uint64_t l1 = L1_INDEX(va);
    uint64_t l2 = L2_INDEX(va);

    /* Without the validity check, it will rewrite the same descriptors.*/
    if (!(l0_table[l0] & PTE_VALID))
    {
        l0_table[l0] = make_table_desc(l1_table);
    }

    if (!(l1_table[l1] & PTE_VALID))
    {
        l1_table[l1] = make_table_desc(l2_table);
    }

    if (!(l2_table[l2] & PTE_VALID))
    {
        l2_table[l2] = make_table_desc(l3_table);
    }
}

static void install_pte(uint64_t va, uint64_t pa, uint64_t attr)
{
    uint64_t l3 = L3_INDEX(va);
    l3_table[l3] = make_page_desc(pa, attr);
}

static void map_page(uint64_t va, uint64_t pa, uint64_t attr)
{
    ensure_page_tables(va);
    install_pte(va, pa, attr);
}

/* 
    One function.
    Many mappings.
*/
static void map_region(uint64_t va_start, uint64_t pa_start, uint64_t size, uint64_t attr)
{
    while (size > 0)
    {
        map_page(va_start, pa_start, attr);
        va_start += PAGE_SIZE;
        pa_start += PAGE_SIZE;
        size -= PAGE_SIZE;
    }
}

static inline void write_mair_el1(uint64_t val)
{
    __asm__ volatile("msr mair_el1, %0" :: "r"(val));
}

static inline void write_tcr_el1(uint64_t val)
{
    __asm__ volatile("msr tcr_el1, %0" :: "r"(val));
}

static inline void write_ttbr0_el1(uint64_t val)
{
    __asm__ volatile("msr ttbr0_el1, %0" :: "r"(val));
}

static inline uint64_t read_sctlr_el1(void)
{
    uint64_t val;

    __asm__ volatile("mrs %0, sctlr_el1"
                     : "=r"(val));

    return val;
}

static inline void write_sctlr_el1(uint64_t val)
{
    __asm__ volatile("msr sctlr_el1, %0"
                     :: "r"(val));
}

static inline void dsb_sy(void)
{
    __asm__ volatile("dsb sy");
}

static inline void isb(void)
{
    __asm__ volatile("isb");
}

/*
    build table of whole region:
*/
void build_translation_tables(void)
{
    map_region(VIRTUAL_ADDRESS, PHYSICAL_ADDRESS, KERNEL_SIZE, NORMAL_MEMORY );
}


void mmu_init(void)
{
    uint64_t sctlr;

    /* Configure memory attributes */
    write_mair_el1(MAIR_VALUE);

    /* Configure translation */
    write_tcr_el1(TCR_VALUE);

    /* Install root page table */
    write_ttbr0_el1((uint64_t)l0_table);

    /* Ensure writes are visible */
    dsb_sy();
    isb();

    /* Enable MMU */
    sctlr = read_sctlr_el1();
    sctlr |= (1UL << 0);

    write_sctlr_el1(sctlr);

    isb();
}