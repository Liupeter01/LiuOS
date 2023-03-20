#define EARLY_ENTRIES(vstart, vend, shift) (((vend) >> (shift)) \
					- ((vstart) >> (shift)) + 1 + EARLY_KASLR)

#define EARLY_PGDS(vstart, vend) (EARLY_ENTRIES(vstart, vend, PGDIR_SHIFT))
#define EARLY_PUDS(vstart, vend) (EARLY_ENTRIES(vstart, vend, PUD_SHIFT))
#define EARLY_PMDS(vstart, vend) (EARLY_ENTRIES(vstart, vend, SWAPPER_TABLE_SHIFT))

#define EARLY_PAGES(vstart, vend) ( 1 			/* PGDIR page */				\
			+ EARLY_PGDS((vstart), (vend)) 	/* each PGDIR needs a next level page table */	\
			+ EARLY_PUDS((vstart), (vend))	/* each PUD needs a next level page table */	\
			+ EARLY_PMDS((vstart), (vend)))	/* each PMD needs a next level page table */
            
#define INIT_DIR_SIZE (PAGE_SIZE * EARLY_PAGES(KIMAGE_VADDR + TEXT_OFFSET, _end))