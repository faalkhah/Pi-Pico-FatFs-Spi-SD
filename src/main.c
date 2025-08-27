#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "ff.h"             // FatFs library
#include "diskio.h"         // FatFs disk I/O

#include "my_diskio.h"
#include "my_sdcard.h"    // SD card driver from previous response
#include "dir.h"
#include "hw_config.h"
#include "read_write.h"

// #define FILE_PATH "/sd/sd_file.txt"
// #define FILE_PATH "0:/sd_file_1.txt"

// SDCard structure instance
SDCard sd;
// FatFs variables
FATFS fs;
FIL fil;
FRESULT fr;
DIR dir;
FILINFO fno;

int main() {
    stdio_init_all();

    sleep_ms(3000);

    hw_init();

    // Initialize SD card
    sdcard_init(&sd, SPI_BUS, CS_PIN, 1320000); // Final baudrate 1.32 MHz

    // Mount file system
    fr = f_mount(&fs, SD_MOUNT_PATH, 1);
    if (fr != FR_OK) {
        printf("Error: Failed to mount SD card (error %d)\n", fr);
        return 1;
    }

    write_file("0:/sd_file_1.txt");
    // List files on SD card
    list_directory(SD_MOUNT_PATH);

    read_file("0:/sd_file_1.txt");

    // Unmount file system
    f_mount(NULL, SD_MOUNT_PATH, 0);

    while (true) {
        tight_loop_contents();
    }
    return 0;
}