#include <stdio.h>
#include <stddef.h> //for size_t
#include <stdint.h> //for uint_t's
#include <stdlib.h>

#include <write.h>
#include <mbr/mbr.h>

void load_mbr() {
    static int tried = 0;

    uint8_t status;
    status = mbr_load();
    if (status == MBR_NOT_VALID) {
        mbr_new();
        if (tried) {
            printf("MBR error: MBR not valid\n");
            exit(EXIT_FAILURE);
        }
        load_mbr();
        tried = 1;
    }
}

int main() {
    mbr_t *mbr;

    drive_init();
    load_mbr();

    mbr = get_mbr();

    uint8_t out[32];
    out[0] = '8';

    mbr_select_partition(PART_1);
    mbr_io(WRITE, out, 0, 0, 1);

    if (mbr->partitions[0].error_code == MBR_EMPTY_PARTITION ||
        mbr->partitions[1].error_code == MBR_EMPTY_PARTITION) 
    {
        printf("MBR EMPTY PARTITIONS!\n");
    }

    uint8_t in[32];
    mbr_io(READ, in, 0, 0, 1);
    printf("got: %d\n", in[0]);

    return EXIT_SUCCESS;
}
