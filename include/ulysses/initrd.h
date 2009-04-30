#ifndef _ULYSSES_INITRD_H
#define _ULYSSES_INITRD_H

/* The initrd is built on top of the VFS and provides the kernel with access
 * to binaries and configuration that isn't linked directly into the kernel.
 * 
 * This should be loaded by the Multiboot loader, and we pick it up and parse
 * it.
 */

#include <ulysses/vfs.h>

/* The header for the initrd */
typedef struct {
    unsigned int nfiles; /* number of files in the ram disk */
} initrd_header_t;

/* A file in the initrd */
typedef struct {
    unsigned char magic; /* for error checking */
    char name[NR_FILENAME];
    unsigned int offset;
    unsigned int length;
} initrd_file_header_t;

/* init_initrd()
 *   Given the memory address given from multiboot, initialises the ram disk 
 *   and returns a completed file system node.
 */
fs_node_t *init_initrd(unsigned int loc);

#endif
