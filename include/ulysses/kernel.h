#ifndef _ULYSSES_KERNEL_H
#define _ULYSSES_KERNEL_H

/* This sets up any global variables (pre-processor definitions should go
 * in /config.h instead) for the kernel.
 */

/* Our own headers */
#include "../../config.h" /* must be first */

#include <ulysses/datetime.h>
#include <ulysses/multiboot.h>

#include <sys/time.h>
#include <sys/types.h>

/* Ensure a valid architecture was declared */
#ifndef _ARCH_x86
#error No _ARCH_* defined!
#endif

/* Kernel stack is 8 KB */
#define STACK_LOC 0xE0000000
#define STACK_SIZE 0x2000

struct kernel_flags {
    flag_t preempt_kernel;
    flag_t debug_sched;
    flag_t debug_task;
    flag_t debug_interrupt;
    flag_t debug_ticks;
};

/* The main kernel data structure */
struct kernel {
    multiboot_info_t *mbi; /* multiboot info struct as passed by loader */
    char *cmdline; /* kernel command line as passed by loader */
    struct kernel_flags flags; /* parsed flags from kernel command line */

    char *cpu_vendor; /* VendorID of CPU */
    char *cpu_model; /* Model name of CPU */
    unsigned int cpu_freq; /* Frequency in MHz of CPU */

    struct datetime startup_datetime;
    struct timeval loaded_time; /* time when kernel was first loaded */
    struct timeval current_time_offset; /* current time since loaded_time */

    pid_t init_pid;
};
extern struct kernel kern;

#endif
