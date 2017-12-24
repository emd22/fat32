#include <stdio.h>
#include <stddef.h> //for size_t
#include <stdint.h> //for uint_t's
#include <stdlib.h>

#include <write.h>
#include <mbr/mbr.h>

#define FAT32 0x0C

void load_mbr(partition_t *parts, size_t b_size) {
    static int tried = 0;

    uint8_t status;
    status = mbr_load();
    if (status == MBR_NOT_VALID) {
        if (tried) {
            printf("MBR error: MBR not valid\n");
            exit(EXIT_FAILURE);
        }
        tried = 1;
        printf("making new mbr\n");
        mbr_new(parts);
        load_mbr(parts, b_size);
    }
}

void write_partitions(partition_t *partitions, size_t b_size) {
    int eq_size = b_size/4;
    int i;
    for (i = 0; i < 4; i++) {
        partitions[i].lba_first_sector = eq_size*i;
        partitions[i].lba_sector_count = eq_size;
        printf("new partition: %d - %d\n", eq_size*i, (eq_size*i)+eq_size);
    }
}

int main() {
    size_t b_size = 12000; //12 KiBs
    partition_t parts[4];

    drive_init();
    format_disk(b_size);
    write_partitions(parts, b_size);
    mbr_new(parts);
    drive_init();

    uint8_t status;

    status = mbr_load();

    if (status != MBR_NO_ERROR)

    // load_mbr(parts, b_size);
    
    set_sys_id(FAT32, PARTITION_ENTRY_2);

    mbr_t *mbr;
    mbr = mbr_get();
    mbr_select_partition(PART_2);

    // uint8_t out[32];
    // out[0] = '8';

    // mbr_io(WRITE, out, 0, 1);

    // uint8_t in[32];
    // mbr_io(READ, in, 0, 1);
    // printf("got: %d\n", in[0]);

    drive_close();

    return EXIT_SUCCESS;
}
