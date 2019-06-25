#include <stdio.h>
#include <stdlib.h>

#include <drive.h>
#include <fat16.h>

int main() {
    drive_init();
    fat16_init();
    // fat16_make_dir("FACJ    EXE");
    // fat16_list_dir();
    drive_close();

    return 0;
}
