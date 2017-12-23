#ifndef WRITE_H
#define WRITE_H

#include <stdint.h>
#include <stddef.h>

void drive_init(void);
void drive_read(uint8_t *buf, size_t pos, size_t size);
void drive_write(uint8_t *buf, size_t pos, size_t size);
void format_disk(size_t bytes);
void drive_close();

#endif