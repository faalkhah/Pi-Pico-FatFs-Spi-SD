/* dir.c
Directory of files
2025 (C)Jalil Faalkhah
*/

#include "dir.h"

void list_directory(const char *path) {
    fr = f_opendir(&dir, path);
    if (fr == FR_OK) {
        printf("Directory contents of %s:\n", path);
        while (true) {
            fr = f_readdir(&dir, &fno);
            if (fr != FR_OK || fno.fname[0] == 0) break;
            printf("  %s\n", fno.fname);
        }
        f_closedir(&dir);
    } else {
        printf("Error: Failed to open directory %s (error %d)\n", path, fr);
    }
}
