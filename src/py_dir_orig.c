#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "ff.h"        // FatFs library
#include "diskio.h"    // FatFs disk I/O
#include "sdcard.c"    // SD card driver from previous response

// Pin definitions from MicroPython script
#define SPI_BUS spi0
#define SCK_PIN 2
#define MOSI_PIN 3
#define MISO_PIN 4
#define CS_PIN 5
#define SD_MOUNT_PATH "/sd"

// SDCard structure instance
SDCard sd;

// FatFs variables
FATFS fs;
FRESULT fr;
DIR dir;
FILINFO fno;

// Disk I/O functions for FatFs
DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv == 0) {
        // Assume SD card is already initialized in main
        return 0;
    }
    return STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv == 0) {
        return 0;
    }
    return STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv == 0) {
        readblocks(&sd, sector, buff, count * 512);
        return RES_OK;
    }
    return RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv == 0) {
        writeblocks(&sd, sector, buff, count * 512);
        return RES_OK;
    }
    return RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv == 0) {
        switch (cmd) {
            case CTRL_SYNC:
                return RES_OK;
            case GET_SECTOR_COUNT:
                *(LBA_t *)buff = ioctl(&sd, 4);
                return RES_OK;
            case GET_SECTOR_SIZE:
                *(WORD *)buff = 512;
                return RES_OK;
            case GET_BLOCK_SIZE:
                *(DWORD *)buff = 1;
                return RES_OK;
        }
    }
    return RES_ERROR;
}

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

int main() {
    stdio_init_all();

    // Initialize SPI pins
    spi_init(SPI_BUS, 100000); // Initial low baudrate
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);

    // Initialize SD card
    sdcard_init(&sd, SPI_BUS, CS_PIN, 1320000); // Final baudrate 1.32 MHz

    // Mount file system
    fr = f_mount(&fs, SD_MOUNT_PATH, 1);
    if (fr != FR_OK) {
        printf("Error: Failed to mount SD card (error %d)\n", fr);
        return 1;
    }

    // List files on SD card
    list_directory(SD_MOUNT_PATH);

    // Unmount file system
    f_mount(NULL, SD_MOUNT_PATH, 0);

    while (true) {
        tight_loop_contents();
    }
    return 0;
}