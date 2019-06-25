#include <drive.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define SECTOR_SIZE 512

static FILE *fp;
const char *fn = "../fs.img";
static size_t drive_size = 0;

void drive_init(void) {
    // open file for binary reading + writing in binary
    fp = fopen(fn, "r+b"); 
    
    if (fp == NULL) {
        printf("error with file %s\n", fn);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    drive_size = ftell(fp);
    rewind(fp);
}

void block_read(unsigned lba, unsigned short sec_count, unsigned char *data) {
    fseek(fp, SECTOR_SIZE*lba, SEEK_SET);
    size_t bytes_read = fread(data, 1, SECTOR_SIZE*sec_count, fp);
    if (bytes_read < sec_count*SECTOR_SIZE) {
        printf("Error: block_read read %lu bytes instead of %lu(LBA: %u)\n", bytes_read, sec_count*SECTOR_SIZE, lba);
        exit(1);
    }
}

void block_write(unsigned lba, unsigned short sec_count, unsigned char *data) {
    fseek(fp, SECTOR_SIZE*lba, SEEK_SET);
    size_t bytes_read = fwrite(data, 1, SECTOR_SIZE*sec_count, fp);
    if (bytes_read < sec_count*SECTOR_SIZE) {
        printf("Error: block_write wrote %lu bytes instead of %lu(LBA: %u)\n", bytes_read, sec_count*SECTOR_SIZE, lba);
        exit(1);
    }
}

void drive_close(void) {
    fclose(fp);
}