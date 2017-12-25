#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <write.h>

// these functions will not be needed
// in an actual os. insert os 
// implementation.

// for anyone who doesn't know 
// how to do disk i/o, look up on 
// how PATA works and build off that.

static FILE *fp;
const char *fn = "test_drive";
static size_t drive_size = 0;

void set_drive_size(void) {
    fseek(fp, 0L, SEEK_END);
    drive_size = ftell(fp);
}

void drive_init(void) {
    // open file for binary reading + appending.

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

    printf("trying to read from %lu\n", pos);
    fseek(fp, pos, SEEK_SET);    
    fread(buf, 1, size, fp);
}

void drive_set_0(void) {
    rewind(fp);
}

void drive_write(uint8_t *buf, size_t pos, size_t size) {
    fseek(fp, pos, SEEK_SET);
    fwrite(buf, 1, size, fp);
}

void format_disk(size_t bytes) {
    puts("formatting disk");

    if (ftruncate(fileno(fp), bytes) != 0) {
        printf("Error truncating file from %lu to %lu\n", drive_size, bytes);
    }

    // uint8_t *zeros = (uint8_t *)calloc(bytes/10, 1);
    // int i;
    // for (i = 0; i < 10; i++) {
    //     fseek(fp, (int)(bytes/10), SEEK_SET);
    //     fwrite(zeros, 1, (int)(bytes/10), fp);
    // }
    // free(zeros);
    drive_size = bytes;
}

void drive_close(void) {
    fclose(fp);
}