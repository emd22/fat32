#include <fat16.h>
#include <drive.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ATTR_READONLY  0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOL_LABEL 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20
#define ATTR_DEVICE    0x40
#define ATTR_RESERVED  0x80

#define FAT16_ROOT_DIR(bpb, ebpb) \
    (bpb->reserved_sectors+bpb->sectors_per_fat*bpb->num_fats)
#define FAT16_FAT_SIZE(bpb)        (bpb->sectors_per_fat)
#define FAT16_ROOT_DIR_SIZE(bpb)   (((bpb->root_entries*32)+(bpb->sector_size-1))/bpb->sector_size)
#define FAT16_DATA_SECTOR(bpb)     (bpb->reserved_sectors+(bpb->num_fats*FAT16_FAT_SIZE(bpb))+FAT16_ROOT_DIR_SIZE(bpb))
#define GET_FIRST_SECTOR_OF_CLUSTER(bpb, cluster) (((cluster-2)*bpb->sectors_per_cluster)+FAT16_DATA_SECTOR(bpb))

typedef struct {
    uint32_t total_sectors;
    uint16_t reserved_sectors;
    uint8_t bit_size;
} fat_settings_t;

typedef struct {
    uint8_t  boot_jmp[3];
    int8_t   oem_name[8];
    uint16_t sector_size;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;
    uint16_t total_sectors0;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t start_lba;
    uint32_t total_sectors1;
} __attribute__ ((packed)) fat_bpb_t;

typedef struct {
    uint8_t drive_number;
    uint8_t reserved0;
    uint8_t signature;
    uint8_t volume_id[4];
    uint8_t volume_label[11];
    uint8_t sys_ident[8];
} __attribute__ ((packed)) fat16_ebpb_t;

typedef struct {
    char     filename[11];
    uint8_t  attributes;
    uint8_t  reserved0;
    uint8_t  create_tenths;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t cluster_hi;
    uint16_t mod_time;
    uint16_t mod_date;
    uint16_t cluster_lo;
    uint32_t file_size;
} __attribute__ ((packed)) dir_entry_t;

fat_bpb_t *bpb;
fat16_ebpb_t *ebpb;

unsigned last_entry_index = 0;
unsigned last_table_block = 0;


char *prettyify_filename(dir_entry_t *entry, char *new_fn) {
    int i;
    char *fn = entry->filename;
    int fn_index = 0;
    for (i = 0; i < 8; i++) {
        if (fn[i] == ' ')
            break;
        new_fn[fn_index++] = fn[i];
    }
    if (!(entry->attributes & ATTR_DIRECTORY)) {
        new_fn[fn_index++] = '.';
        for (i = 0; i < 3; i++)
            new_fn[fn_index++] = fn[i+8];
    }
    new_fn[fn_index++] = 0;
    return new_fn;
}
void print_entry_info(dir_entry_t *entry) {
    char new_fn[12];

    printf("Entry %.11s:\n", prettyify_filename(entry, new_fn));
    printf("\tAttributes: \n", entry->attributes);
    
    if (entry->attributes & ATTR_READONLY)  printf("\t\tRead Only\n");
    if (entry->attributes & ATTR_HIDDEN)    printf("\t\tHidden\n");
    if (entry->attributes & ATTR_SYSTEM)    printf("\t\tSystem\n");
    if (entry->attributes & ATTR_VOL_LABEL) printf("\t\tVolume Label\n");
    if (entry->attributes & ATTR_DIRECTORY) printf("\t\tDirectory\n");
    if (entry->attributes & ATTR_ARCHIVE)   printf("\t\tArchive(File)\n");
    if (entry->attributes & ATTR_DEVICE)    printf("\t\tDevice\n");
    if (entry->attributes & ATTR_RESERVED)  printf("\t\tReserved\n");

    printf("\tFile size: %u Bytes\n", entry->file_size);
    printf("\tCluster Location:\n", entry->filename);
    printf("\t\tLo: %u\n", entry->cluster_lo);
    printf("\t\tHi: %u\n", entry->cluster_hi);
}

// dir_entry_t *write_new_entry(dir_entry_t *entry) {
//     dir_entry_t sector[16];
//     block_read(last_table_block);



//     return entry;
// }

char *fat16_read_file(dir_entry_t *entry) {
    // unsigned amt_sectors = entry->file_size/512;
    // unsigned sec_start = GET_FIRST_SECTOR_OF_CLUSTER(bpb, entry->cluster_lo);
    // if (entry->file_size % 512)
    //     amt_sectors++;

    char *data = malloc(entry->file_size);

    int i;
    uint8_t sector[512];
    unsigned block_data_idx = 0;
    unsigned blocks_idx = GET_FIRST_SECTOR_OF_CLUSTER(bpb, entry->cluster_lo);
    
    for (i = 0; i < entry->file_size; i++) {
        if (!(i % 512)) {
            block_read(blocks_idx++, 1, sector);
            block_data_idx = 0;
        }
        data[i] = sector[block_data_idx++];
    }

    return data;
}

void fat16_list_dir() {
    dir_entry_t entries[16];
    int lba = FAT16_ROOT_DIR(bpb, ebpb);
    int i;
    int entries_idx = 0;
    char new_fn[12];

    for (i = 0; i < bpb->root_entries; i++) {
        if (!(i*sizeof(dir_entry_t) % 512)) {
            block_read(lba++, 1, (uint8_t *)entries);
            entries_idx = 0;
        }
        if (entries[entries_idx].filename[0] == 0)
            continue;

        printf("FN: %.11s\n", prettyify_filename(&entries[entries_idx++], new_fn));
    }
}

void fat16_init() {
    uint8_t *bs = malloc(512);
    block_read(0, 1, bs);
    bpb = (fat_bpb_t *)bs;
    ebpb = (fat16_ebpb_t *)(bs+sizeof(fat_bpb_t));

    printf("fs rootdir: %d\n", FAT16_ROOT_DIR(bpb, ebpb));
    printf("fs rootdir entries: %d\n", bpb->root_entries);

    dir_entry_t entries[16];
    int lba = FAT16_ROOT_DIR(bpb, ebpb);
    int i;
    int entries_idx = 0;
    char new_fn[12];
    // char *data;
    for (i = 0; i < bpb->root_entries; i++) {
        if (!(i*sizeof(dir_entry_t) % 512)) {
            block_read(lba++, 1, (uint8_t *)entries);
            entries_idx = 0;
        }
        if (entries[entries_idx].filename[0] == 0)
            continue;
        last_entry_index = entries_idx*sizeof(dir_entry_t);
        // data = fat16_read_file(&entries[entries_idx]);
        print_entry_info(&entries[entries_idx++]);
        // printf("data: %s\n", data);
    }
    last_table_block = lba;
}