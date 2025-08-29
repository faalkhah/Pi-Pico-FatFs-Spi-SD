/* main.c
main file 
2025 (C)Jalil Faalkhah 
*/

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
#include "debug_led_status.h"

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

    sleep_ms(2000);

    hw_init();
 // برای دیباگ
    led_status_init(PICO_DEFAULT_LED_PIN);

    // Initialize SD card
    sdcard_init(&sd, SPI_BUS, CS_PIN, 1320000); // Final baudrate 1.32 MHz

    // Mount file system
    fr = f_mount(&fs, SD_MOUNT_PATH, 1);
    if (fr != FR_OK) {
        printf("Error: Failed to mount SD card (error %d)\n", fr);
        led_status_set(LED_BLINK_FAST); // Error
        while (1) {
            led_status_update();
        }
        // return 1;
    }

    printf("=== Mount SD Card success!\n");
    led_status_set(LED_ON); // Write successful, turn LED on
    // Update LED state for 1 second
    for (int i = 0; i < 100; i++) {
        led_status_update();
        sleep_ms(10); // 100 * 10ms = 1 second
    }
    led_status_set(LED_OFF);

    write_file("0:/hello_world.txt");
    printf("=== Writing hello_world.txt sucess!\n");
    // List files on SD card
    list_directory(SD_MOUNT_PATH);
    printf("=== Reading hello_world.txt ...\n");
    read_file("0:/hello_world.txt");
    // Unmount file system
    f_mount(NULL, SD_MOUNT_PATH, 0);
    printf("=== Disk unmount success!\n");
    while (true) {
        led_status_update();
        sleep_ms(10);  // Adjust delay as needed
        // tight_loop_contents();
    }
    return 0;
}