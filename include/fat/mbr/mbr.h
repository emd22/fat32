// #ifndef MBR_H
// #define MBR_H

// #include <stddef.h>
// #include <stdint.h>

// #define MBR_NO_ERROR  0
// #define MBR_NO_SYS_ID 1
// #define MBR_FOUND_EBR 2
// #define MBR_NOT_VALID 3

// /*
// these are the locations which each partition is located.
// */
// enum {
//     PARTITION_ENTRY_1 = 0x1BE,
//     PARTITION_ENTRY_2 = 0x1CE,
//     PARTITION_ENTRY_3 = 0x1DE,
//     PARTITION_ENTRY_4 = 0x1EE
// };

// enum RW_TYPE {
//     RW_NONE,
//     READ,
//     WRITE,
// };

// enum SELECTED_PART {
//     SP_NONE,
//     PART_1,
//     PART_2,
//     PART_3,
//     PART_4
// };

// typedef struct {
//     uint8_t     code;
//     const char *msg;
// } mbr_error_t;

// typedef struct {
//     mbr_error_t error;
//     uint8_t     bootable;
//     uint8_t     type;
//     unsigned    lba_first_sector;
//     unsigned    lba_sector_count;
//     /*
//     just for ease of use 
//     (you can just do lba_first_sector+lba_sector_count)
//     */
//     unsigned    lba_end_sector; 
// } partition_t;

// typedef struct {
//     char        bootstrap[446];
//     partition_t partitions[4];
//     uint8_t     signature[2];
// } mbr_t;

// int mbr_load(void);
// void mbr_select_partition(enum SELECTED_PART part_n);
// void mbr_io(enum RW_TYPE type, uint8_t *buffer, size_t lba, size_t len);
// mbr_t *mbr_get(void);
// void mbr_new(partition_t *partitions);
// void set_sys_id(uint8_t sys_id, int position);

// #endif