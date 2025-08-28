/* dir.h
Directory of files
2025 (C)Jalil Faalkhah
*/
#ifndef DIR_H
#define DIR_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "ff.h"        // FatFs library
#include "diskio.h"    // FatFs disk I/O

// FatFs variables
// FATFS fs;
extern FRESULT fr;
extern DIR dir;
extern FILINFO fno;

void list_directory(const char *path);

#endif