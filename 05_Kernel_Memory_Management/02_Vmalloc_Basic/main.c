#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <asm/io.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

/* Number of pages to allocate via vmalloc for the contiguity test */
#define VMALLOC_TEST_PAGES   16
#define VMALLOC_TEST_SIZE    (VMALLOC_TEST_PAGES * PAGE_SIZE)

static char *kbuffer  = NULL;   /* kmalloc buffer  - physically contiguous */
static char *vbuffer  = NULL;   /* vmalloc buffer  - virtually contiguous only */

static void check_vmalloc_contiguity(void *vbuf, size_t size)
{
    unsigned long addr   = (unsigned long)vbuf;
    unsigned long offset;
    unsigned long prev_pfn = 0;
    int page_num = 0;
    int discontig_count = 0;

    pr_info("---- vmalloc physical contiguity check (%zu bytes, %zu pages) ----\n",
             size, size / PAGE_SIZE);

    for (offset = 0; offset < size; offset += PAGE_SIZE)
    {
        struct page *pg;
        unsigned long pfn;
        phys_addr_t phys;
        bool contig;
/*
    vmalloc_to_page(addr) → returns the struct page * backing that virtual address
    page_to_pfn(page) → the physical page frame number
    PFN_PHYS(pfn) → converts pfn to an actual physical address (pfn << PAGE_SHIFT)\

    PFN = Physical Address >> PAGE_SHIFT
    Physical Address = PFN << PAGE_SHIFT
    PAGE_SHIFT = 12, as PAGE_SIZE = 4096 = 2^12
*/
        pg  = vmalloc_to_page((void *)(addr + offset));
        if (!pg)
        {
            pr_err("vmalloc_to_page() failed at offset %lu\n", offset);
            continue;
        }

        pfn  = page_to_pfn(pg);
        phys = PFN_PHYS(pfn);

        //This varibale to count num of discontinuities.
        contig = ((page_num == 0) || (pfn == prev_pfn + 1));

        pr_info("page %2d: vaddr=%px  pfn=%lu  phys=%llx%s\n",
                 page_num,
                 (void *)(addr + offset),
                 pfn,
                 (unsigned long long)phys,
                 contig ? "" : "  <-- NOT contiguous with previous page");

        if (!contig)
        {
            discontig_count++;
        }

        prev_pfn = pfn;
        page_num++;
    }

    pr_info("---- result: %d discontinuity(ies) found across %d pages ----\n",
             discontig_count, page_num);
}

static void check_kmalloc_contiguity(void *kbuf, size_t size)
{
    unsigned long addr = (unsigned long)kbuf;
    unsigned long offset;

    phys_addr_t base_phys = virt_to_phys((void *)addr);

    pr_info("---- kmalloc physical contiguity check (%zu bytes) ----\n", size);
    pr_info("base virt=%px  base phys=%llx\n", 
             (void *)addr, (unsigned long long)base_phys);

    for (offset = 0; offset < size; offset += PAGE_SIZE)
    {
        phys_addr_t phys = virt_to_phys((void *)(addr + offset));
        phys_addr_t expected = base_phys + offset;

        pr_info("offset %5lu: virt=%px  phys=%llx%s\n",
                 offset, (void *)(addr + offset),
                 (unsigned long long)phys,
                 (phys == expected) ? "" : "  <-- unexpected gap!");
    }
}

static int __init module_driver_init(void)
{
    int ret = 0;
    pr_info("Module Driver Init Called\n");

    /* ---- kmalloc: physically AND virtually contiguous ---- */
    kbuffer = kmalloc(VMALLOC_TEST_SIZE, GFP_KERNEL);
    if (!kbuffer)
    {
        pr_err("kmalloc allocation failed\n");
        return -ENOMEM;
    }
    check_kmalloc_contiguity(kbuffer, VMALLOC_TEST_SIZE);


    /* ---- vmalloc: virtually contiguous only ---- */
    vbuffer = vmalloc(VMALLOC_TEST_SIZE);
    if (!vbuffer) 
    {
        pr_err("vmalloc allocation failed\n");
        ret =  -ENOMEM;
        goto free_kbuffer;
    }

    check_vmalloc_contiguity(vbuffer, VMALLOC_TEST_SIZE);
    ret = 0;
    goto end;

free_kbuffer:
    kfree(kbuffer);
    kbuffer = NULL;
end:
    return ret;
}

static void __exit module_driver_clean(void)
{
    pr_info("kmalloc_lab: freeing memory\n");

    if (kbuffer)
    {
        kfree(kbuffer);     /* kmalloc memory -> kfree */
    }

    if (vbuffer)
    {
        vfree(vbuffer);     /* vmalloc memory -> vfree (NEVER kfree) */
    }

    pr_info("Module Driver Clean Up Called\n");
}

module_init(module_driver_init);
module_exit(module_driver_clean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raj Kumar Mahto");
MODULE_DESCRIPTION("Demonstrates kmalloc vs vmalloc physical (dis)contiguity");
