
#ifndef _IDT_H
#define _IDT_H

#include <types.h>

struct idt_entry_struct {
    unsigned short base_low; /* lower 16 bits to jump to */
    unsigned short sel; /* segment selector */
    unsigned char always0; /* just as the name says... */
    unsigned char flags;
    unsigned short base_high; /* higher 16 bits to jump to */
} __attribute__((packed)); /* pack to 1 byte */
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)); /* pack to 1 byte */
typedef struct idt_ptr_struct idt_ptr_t;

idt_ptr_t idt_ptr;
idt_entry_t idt_entries[256]; /* that's a lot of interrupts */

/* init_idt()
 *  Initialise the interrupt descriptor table.
 */
flag_t init_idt(void);

/* See flush.s for assembler routine for this */
/* XXX move this to inline assembler */
extern void idt_flush(unsigned int);

/* These are the interrupt handler stubs. They are declared in interrupts.s.
 * This is very messy, since the CPU has the possibility of generating 32
 * interrupts (and will triple fault if not handler exists for them).
 *
 * FYI isr = interrupt service routine
 */
extern void isr0(); /* div by zero */
extern void isr1(); /* debug */
extern void isr2(); /* non-maskable interrupt */
extern void isr3(); /* breakpoint */
extern void isr4(); /* overflow */
extern void isr5(); /* out of bounds */
extern void isr6(); /* invalid opcode */
extern void isr7(); /* no coprocessor */
extern void isr8(); /* double fault (w/ error code) */
extern void isr9(); /* coprocessor segment overrun */
extern void isr10(); /* bad tss (w/ error code) */
extern void isr11(); /* segment not present (w/ error code) */
extern void isr12(); /* stack fault (w/ error code) */
extern void isr13(); /* general protection fault (w/ error code) */
extern void isr14(); /* page fault (w/ error code) */
extern void isr15(); /* uknown interrupt */
extern void isr16(); /* coprocessor fault */
extern void isr17(); /* alignment check */
extern void isr18(); /* machine exception */

/* the rest are reserved... */
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* phew. now look in interrupts.s */

#endif

