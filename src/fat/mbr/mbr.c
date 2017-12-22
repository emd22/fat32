#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <mbr/mbr.h>
#include <write.h>
#include <fakeos.h>

static mbr_t mbr;
static int   def_part;

void mbr_init(void) {
    int i;
    for (i = 0; i < 4; i++) {
        /*
        MBR has a maximum of four partitions.
        this initializes each one for later use.
        to have more partitions use EBR.
        */
        mbr.partitions[i].error_code = 0;
    }
    def_part = 0;
}

/*
convert uint8_t to uint32_t
*/
uint32_t to_uint32(uint8_t *n, unsigned pos) {
    return (unsigned)n[pos+3] << 24 | (unsigned)n[pos+2] << 16 |
           (unsigned)n[pos+1] << 8  | n[pos];
}


/* 
create a new MBR on disk
(file in this case)
*/
void mbr_new(void) {
    uint8_t mbr_head[512];

    //setting mbr signature
    mbr_head[510] = 0x55;
    mbr_head[511] = 0xAA;

    drive_write(mbr_head, 0, 512);
}

void parse_partition(int part_n, uint8_t *mbr_head, size_t location) {
    partition_t *part = &mbr.partitions[part_n];
    char sys_id = mbr_head[location+4];

    if (!sys_id) {
        part->error_code = MBR_EMPTY_PARTITION;
    }

    if (sys_id == 0x50 || sys_id == 0xF0 || sys_id == 0x85) {
        part->error_code = MBR_FOUND_EBR;
    }
    else {
        part->type = sys_id;
        part->lba_first_sector = to_uint32(mbr_head, location+8);
        part->lba_sector_count = to_uint32(mbr_head, location+12);
        part->lba_end_sector = part->lba_first_sector+part->lba_sector_count;
        part->bootable = mbr_head[location] == 0x81;
    }
}

int mbr_load(void) {
    uint8_t mbr_head[512];
    
    drive_read(mbr_head, 0, 512);

    mbr.signature[0] = mbr_head[510];
    mbr.signature[1] = mbr_head[511];

    if (mbr.signature[0] != 0x55 || mbr.signature[1] != 0xAA) {
        return MBR_NOT_VALID;
    }

    // optional: copy bootstrap just in case of later use
    strncpy(mbr.bootstrap, mbr_head, 446);

    //try and read partitions

    int i;
    for (i = 0; i < 4; i++)
        parse_partition(i, mbr_head, PARTITION_ENTRY_1+(i*10));
    
    return 0;
}

void mbr_select_partition(enum SELECTED_PART part_n) {
    def_part = part_n;
}

void mbr_io(enum RW_TYPE type, uint8_t *buffer, size_t lba, size_t sector_len, size_t precise_len) {
    size_t len = 0;

    switch (type) {
        case WRITE:
        case READ:
            len = precise_len;
            break;
        case WRITE_SECTORS:
        case READ_SECTORS:
            len = sector_len*512;
            break;
    }

    if (type == READ || type == READ_SECTORS) {
        //read
        drive_read(buffer, mbr.partitions[def_part].lba_first_sector+lba, len);
    }
    else {
        //write
        drive_write(buffer, mbr.partitions[def_part].lba_first_sector+lba, len);
    }
}

mbr_t *get_mbr(void) {
    return &mbr;
}