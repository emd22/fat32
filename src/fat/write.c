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
static size_t drive_size = 0;

void set_drive_size(void) {
    size_t old = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    drive_size = ftell(fp);
    fseek(fp, old, SEEK_SET);
}

void drive_init(void) {
    // open file for binary reading + appending.
    const char *fn = "test_drive";

    fp = fopen(fn, "a+b"); 
    
    if (fp == NULL) {
        printf("error with file %s\n", fn);
        exit(EXIT_FAILURE);
    }

    set_drive_size();
    fseek(fp, 0, SEEK_SET);
}

void drive_read(uint8_t *buf, size_t pos, size_t size) {
    if (pos > drive_size || pos < 0) {
        memset(buf, 0, size);
        return;
    }
    fseek(fp, pos, SEEK_SET);    
    fread(buf, 1, size, fp);
}

void drive_write(uint8_t *buf, size_t pos, size_t size) {
    if (pos != drive_size)
        fseek(fp, pos, SEEK_SET);
    else
        fseek(fp, 0, SEEK_END);
    fwrite(buf, 1, size, fp);
}

void format_disk(size_t bytes) {
    static const char zeros[4096];
    size_t size = fseek(fp, 0, SEEK_END);
    fseek(fp, 0, SEEK_SET);
    while (size > sizeof zeros)
        size -= fwrite(zeros, 1, sizeof zeros, fp);
    while (size)
        size -= fwrite(zeros, 1, size, fp);
}

void drive_close() {
    fclose(fp);
}