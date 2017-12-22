#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <write.h>

// these functions will not be needed
// in an actual os. insert os 
// implementation.

// for anyone who doesn't know 
// how to do disk i/o, look up on 
// how PATA works and build off that.

static FILE *fp;
static int drive_size = 0;

void drive_init(void) {
    // open file for binary reading + writing without overwrite.
    const char *fn = "test_drive";

    fp = fopen(fn, "a+b"); 
    if (fp == NULL) {
        printf("error with file %s\n", fn);
        exit(1);
    }
}

void drive_read(uint8_t *buf, size_t pos, size_t size) {
    fseek(fp, pos, SEEK_SET);    
    fread(buf, 1, size, fp);
}

void get_drive_size(void) {
    size_t old = ftell(fp);
    fseek(fp, 0, SEEK_END);
    drive_size = ftell(fp);
    fseek(fp, old, SEEK_SET);
}

void drive_write(uint8_t *buf, size_t pos, size_t size) {
    fseek(fp, pos, SEEK_SET);
    fwrite(buf, 1, size, fp);
}

void format_disk(size_t bytes) {
    //+1 sector for MBR header
    size_t sector_amt = (bytes/512)+1;

    uint8_t *dat = (uint8_t *)malloc(sector_amt);
    memset(dat, 0, sector_amt);

    int i;
    for (i = 0; i < 512; i++) {
        drive_write(dat, i*sector_amt, sector_amt);
    }
    free(dat);
}