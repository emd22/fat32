#ifndef FAT_H
#define FAT_H

void fat16_init(void);
void fat16_list_dir(void);
void fat16_make_dir(char *fn);

#endif