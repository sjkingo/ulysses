#ifndef _ULYSSES_INITRD_H
#define _ULYSSES_INITRD_H

/* The initrd is built on top of the VFS and provides the kernel with access
 * to binaries and configuration that isn't linked directly into the kernel.
 * 
 * This should be loaded by the Multiboot loader, and we pick it up and parse
 * it.
 */

#define INITRD_FILENAME_LEN 64

/* The header for the initrd */
typedef struct {
    unsigned int nfiles; /* number of files in the ram disk */
} initrd_header_t;

/* A file in the initrd */
typedef struct {
    unsigned char magic; /* for error checking */
    char name[INITRD_FILENAME_LEN];
    unsigned int offset;
    unsigned int length;
} initrd_file_header_t;

struct file {
    char *name;
    unsigned int size;
    void *data;
};

/* run_initrd()
 *  Runs the initrd, setting up root filesystem.
 */
void run_initrd(void);

struct file *load_file(char *name);

#endif
