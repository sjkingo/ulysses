/* arch/x86/drivers/disk/ata.c - ATA driver
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>
#include <ulysses/util_x86.h>

#include <ulysses/drivers/disk/ata.h>

#include <sys/types.h>
#include <unistd.h>

#define NUM_DRIVES 2
static struct drive *drives[NUM_DRIVES];

static flag_t ataInit = FALSE;

/* init_drive()
 *  Initialises the given drive and returns a pointer to its struct.
 */
static struct drive *init_drive(int drive)
{
    int status;
    struct drive *d = kmalloc(sizeof(struct drive));

    /* Set the current drive */
    if (drive == 0) {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_0);
    } else if (drive == 1) {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_1);
    } else {
        return NULL; /* no such controller */
    }

    outb(ATA_COMMAND_REG, ATA_COMMAND_IDENTIFY_DRIVE);
    wait_for_drive();

    status = inb(ATA_STATUS_REG);
    if ((status & ATA_STATUS_DRIVE_DATA_REQ)) {
        /* Drive responded, get its info */
        int i;
        short info[256];

        /* Query the controller for disk information */
        for (i = 0; i < 256; i++) {
            info[i] = inw(ATA_DATA_REG);
        }
        
        /* Populate the struct */
        d->num = drive;
        d->cyls = info[ATA_ATANT_CYLS];
        d->heads = info[ATA_ATANT_HEADS];
        d->sectors = info[ATA_ATANT_SPT];
        d->bytes_per_sect = info[ATA_ATANT_BPS];
        d->size = d->cyls * d->heads * d->sectors * d->bytes_per_sect;
        drives[drive] = d;
        return d;
    } else {
        /* XXX Could be ATAPI */
        kfree(d);
        return NULL;
    }
}

/* num_blocks()
 *  Calculates and returns the number of blocks on the drive.
 */
static inline int num_blocks(struct drive *d)
{
    return d->cyls * d->heads * d->sectors;
}

static flag_t verify_null(const char *buf, size_t len)
{
    unsigned short i;
    for (i = 0; i < len; i++) {
        if (buf[i] != '\0') {
            return FALSE;
        }
    }
    return TRUE;
}

int read_ata(struct drive *d, int block, char *buffer, size_t size)
{
    int sector, cyl, head;
    unsigned short i;
    short *buf = (short *)buffer;

    if (block < 0 || block >= num_blocks(d)) {
        kprintf("Invalid block %d\n", block);
        return FALSE;
    }

    /* Calculate where the block lies on disk */
    sector = block % d->sectors + 1;
    cyl = block / (d->heads * d->sectors);
    head = (block / d->sectors) % d->heads;

    /* Seek the disk */
    outb(ATA_SECTOR_COUNT_REG, 1);
    outb(ATA_SECTOR_NUM_REG, sector);
    outb(ATA_CYL_LOW_REG, LOW_BYTE(cyl));
    outb(ATA_CYL_HIGH_REG, HIGH_BYTE(cyl));
    if (d->num == 0) {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_0 | head);
    } else {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_1 | head);
    }
    outb(ATA_COMMAND_REG, ATA_COMMAND_READ_SECTORS);
    wait_for_drive();

    /* Check for error in seeking */
    if (inb(ATA_STATUS_REG) & ATA_STATUS_DRIVE_ERROR) {
        kprintf("error reading: %d\n", inb(ATA_STATUS_REG));
        return FALSE;
    }

    /* Populate the buffer */
    for (i = 0; i < 256; i++) {
        buf[i] = inw(ATA_DATA_REG);
    }

    return TRUE;
}

void init_ata(void)
{
    if (ataInit) panic("init_ata() already called");
    ataInit = TRUE;

    /* Reset the controller */
    outb(ATA_DEVICE_CONTROL_REG, ATA_DCR_NOINT | ATA_DCR_RESET);
    outb(ATA_DEVICE_CONTROL_REG, ATA_DCR_NOINT);
    wait_for_drive();

    unsigned short i;
    for (i = 0; i < NUM_DRIVES; i++) {
        drives[i] = NULL;
    }

    init_drive(0);
}

void dump_drives(void)
{
    unsigned short i;
    for (i = 0; i < NUM_DRIVES; i++) {
        if (drives[i] == NULL) {
            kprintf("ata%d: not connected\n", i);
        } else {
            kprintf("ata%d: %ldM (%d blocks)\n", i, 
                    BYTES_TO_MEGS(drives[i]->size), num_blocks(drives[i]));
        }
    }
}

void ata_worker(void)
{
    if (!ataInit) kthread_exit();
    while (1) {
        kthread_yield();
    }
}
