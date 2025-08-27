#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "ff.h"        // FatFs library
#include "diskio.h"    // FatFs disk I/O

#define FILE_PATH "0:/sd_file_1.txt"

extern FIL fil;
extern FRESULT fr;

void read_file(const char *path);
void write_file(const char *path);

#endif