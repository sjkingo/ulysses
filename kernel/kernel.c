
#include "kernel.h"

void kmain(void *mdb, unsigned int magic)
{
    unsigned char *vid = (unsigned char *)VIDMEM;
    vid[0] = 65; /* 'A' */
    vid[1] = 0x07; /* white on black */
}

