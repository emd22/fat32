#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <write.h>

// this block will not be needed
// in an actual os. insert 
// os implementation.

// for anyone who doesn't know 
// how to do disk i/o, look up on 
// how PATA works and build off that.

static FILE *partition;

void drive_init(void) {
    // open file for binary reading + writing without overwrite.
    partition = fopen("test_drive", "a+b"); 
}

void drive_read(uint8_t *buf, size_t pos, size_t size) {
    fseek(partition, pos, SEEK_SET);    
    fread(buf, 1, size, partition);
}

void drive_write(uint8_t *buf, size_t pos, size_t size) {
    fseek(partition, pos, SEEK_SET);
    fwrite(buf, 1, size, partition);
}

// end of block