#ifndef WRITE_H
#define WRITE_H

#include <stdint.h>
#include <stddef.h>

void drive_init(void);
void block_read(unsigned lba, unsigned short sec_count, unsigned char *data);
void block_write(unsigned lba, unsigned short sec_count, unsigned char *data);
void drive_close(void);

#endif