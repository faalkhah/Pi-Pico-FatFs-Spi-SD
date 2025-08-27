#include "my_sd_card.c"

int main() {
    stdio_init_all();
    SDCard sd;
    sdcard_init(&sd, spi0, 5, 1320000); // CS pin 5, baudrate 1.32 MHz
    uint8_t buf[512];
    readblocks(&sd, 0, buf, 512); // Read first block
    printf("Sectors: %lu\n", ioctl(&sd, 4));
    while (true) tight_loop_contents();
}