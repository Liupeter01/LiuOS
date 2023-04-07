.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.globl get_current_el
get_current_el:
    mrs x0, currentel
    mov x1, #2
    lsr x0, x0, #2
    ret

.globl jump_to_el0
jump_to_el0:
    mov x1, x0
    mrs x0, currentel       // check if in el2
    cmp x0, #8
    beq 2f

    // ldr x0, =0xe00000
    // msr sp_el0, x0          // init the stack of el0

    // return to el0 from el1
    mov x0, #0x0
    msr spsr_el1, x0        // el0 with DAIF = 0
    msr elr_el1, x1
    eret
2:
    ret

.globl tlb_invalidate
tlb_invalidate:
    dsb ishst               // ensure write has completed
    tlbi vmalle1is          // invalidate tlb, all asid, el1.
    dsb ish                 // ensure completion of tlb invalidation
    isb                     // synchronize context and ensure that no instructions
                            // are fetched using the old translation
    ret

// dcahe_invalidate(void *va_start, void *va_end)
.global dcache_invalidate
dcache_invalidate:
        // flush D
        mrs x2, ctr_el0
        ubfx x4, x2, #16, #4
        mov x3, #4
        lsl x3, x3, x4
        sub x4, x3, #1
        bic x4, x0, x4
dci:
        dc civac, x4
        add x4, x4, x3
        cmp x4, x1
        b.lt dci
        
        dsb ish
        isb

        ret

// cache_flush(void *va_start, void *va_end)
.global cache_flush
cache_flush:
        // flush D
        mrs x2, ctr_el0
        ubfx x4, x2, #16, #4
        mov x3, #4
        lsl x3, x3, x4
        sub x4, x3, #1
        bic x4, x0, x4
cdc:
        dc cvau, x4
        add x4, x4, x3
        cmp x4, x1
        b.lt cdc
        
        dsb ish

        // flush I
        and x2, x2, #0xf
        mov x3, #4
        lsl x3, x3, x2
        sub x4, x3, #1
        bic x4, x0, x4
cic:
        ic ivau, x4
        add x4, x4, x3
        cmp x4, x1
        b.lt cic

        dsb ish
        isb

        ret


.globl put32
put32:
    str w1,[x0]
    ret

.globl get32
get32:
    ldr w0,[x0]
    ret

.globl dummy
dummy:
    ret

.globl load_ttbr0_context
load_ttbr0_context:
    msr ttbr0_el1, x0
    ret

.globl load_ttbr1_context
load_ttbr1_context:
    msr ttbr1_el1, x0
    ret

.globl get_sctlr_el1
get_sctlr_el1:
    mrs x0, sctlr_el1
    ret

.globl get_esr
get_esr:
    mrs x0, esr_el1
    ret

.globl get_spsr
get_spsr:
    mrs x0, spsr_el1
    ret

.globl get_elr
get_elr:
    mrs x0, elr_el1
    ret

.globl get_far
get_far:
    mrs x0, far_el1
    ret