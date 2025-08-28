/* hw_config.h
Set gpio pins
2025 (C)Jalil Faalkhah
*/

#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "ff.h"        // FatFs library
#include "diskio.h"    // FatFs disk I/O
#include "my_diskio.h"
#include "my_sdcard.h"    // SD card driver from previous response

// Pin definitions from MicroPython script
#define SPI_BUS spi0
#define SCK_PIN 2
#define MOSI_PIN 3
#define MISO_PIN 4
#define CS_PIN 5
// #define SD_MOUNT_PATH "/sd"
#define SD_MOUNT_PATH "0:"

void hw_init();

#endif