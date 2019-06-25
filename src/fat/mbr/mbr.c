// #include <stdint.h>
// #include <string.h>
// #include <stdio.h>

// #include <mbr/mbr.h>
// #include <write.h>
// #include <fakeos.h>

// static mbr_t mbr;
// static int   def_part;

// void mbr_error(int code, int part) {
//     const char err_strs[][32] = {
//         "No error", 
//         "Sys id not found", 
//         "EBR partitions not supported",
//         "MBR header not valid"
//     };

//     mbr_error_t mbr_err;
//     mbr_err.code = code;
//     mbr_err.msg  = err_strs[code];

//     mbr.partitions[part].error = mbr_err;
// }

// void mbr_init(void) {
//     int i;
//     for (i = 0; i < 4; i++) {
//         /*
//         MBR has a maximum of four partitions.
//         this initializes each one for later use.
//         to have more partitions use EBR.
//         */
//         //set all paritions error settings to no error
//         mbr_error(MBR_NO_ERROR, i);
//     }
//     def_part = 0;
// }

// /*
// convert uint8_t to uint32_t
// */
// uint32_t to_uint32(uint8_t *n, unsigned pos) {
//     return (unsigned)n[pos+3] << 24 | (unsigned)n[pos+2] << 16 |
//            (unsigned)n[pos+1] << 8  | n[pos];
// }

// /* 
// create a new MBR on disk
// (file in this case)
// */
// void mbr_new(partition_t *partitions) {
//     uint8_t mbr_head[512];

//     memset(mbr_head, 0, 512);

//     //setting mbr signature
//     mbr_head[510] = 0x55;
//     mbr_head[511] = 0xAA;

//     int i;
//     for (i = 0; i < 4; i++) {
//         mbr_head[PARTITION_ENTRY_1+(i*10)+8] = partitions[i].lba_first_sector;
//         mbr_head[PARTITION_ENTRY_1+(i*10)+12] = partitions[i].lba_sector_count;
//     }
//     drive_write(mbr_head, 0, 512);
// }

// void parse_partition(int part_n, uint8_t *mbr_head, size_t location) {
//     partition_t *part = &mbr.partitions[part_n];
//     char sys_id = mbr_head[location+4];

//     if (!sys_id) {
//         printf("Partition %d is empty.\n", part_n+1);
//         mbr_error(MBR_NO_SYS_ID, part_n);
//     }

//     if (sys_id == 0x50 || sys_id == 0xF0 || sys_id == 0x85) {
//         mbr_error(MBR_FOUND_EBR, part_n);
//     }

//     else {
//         part->type = sys_id;
//         part->lba_first_sector = to_uint32(mbr_head, location+8);
//         part->lba_sector_count = to_uint32(mbr_head, location+12);
//         part->lba_end_sector = part->lba_first_sector+part->lba_sector_count;
//         part->bootable = mbr_head[location] == 0x81;
//     }

//     if (!part->error.code) {
//         printf("Usable partition: %d - %d\n", part->lba_first_sector, part->lba_end_sector);
//     }
//     else {
//         printf("Error: %s\n", part->error.msg);
//     }
// }

// int mbr_load(void) {
//     uint8_t mbr_head[512];
    
//     drive_read(mbr_head, 0, 512);

//     mbr.signature[0] = mbr_head[510];
//     mbr.signature[1] = mbr_head[511];

//     if (mbr.signature[0] != 0x55 || mbr.signature[1] != 0xAA) {
//         return MBR_NOT_VALID;
//     }

//     // optional: copy bootstrap just in case of later use
//     strncpy(mbr.bootstrap, mbr_head, 446);

//     //try and read partitions

//     int i;
//     for (i = 0; i < 4; i++) {
//         parse_partition(i, mbr_head, PARTITION_ENTRY_1+(i*10));
//     }
    
//     return 0;
// }

// void mbr_select_partition(enum SELECTED_PART part_n) {
//     def_part = part_n;
// }

// void mbr_io(enum RW_TYPE type, uint8_t *buffer, size_t lba, size_t len) {
//     if (type == READ) {
//         //read
//         drive_read(buffer, mbr.partitions[def_part].lba_first_sector+512+lba, len);
//     }
//     else {
//         //write
//         drive_write(buffer, mbr.partitions[def_part].lba_first_sector+512+lba, len);
//     }
// }

// mbr_t *mbr_get(void) {
//     return &mbr;
// }

// void set_sys_id(uint8_t sys_id, int position) {
//     puts("writing sys_id");
//     mbr_io(WRITE, &sys_id, position+4, 1);
// }