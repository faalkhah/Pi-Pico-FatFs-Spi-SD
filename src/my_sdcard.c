#include <stdio.h>
#include <string.h>
#include "my_sdcard.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Initialize SPI with specified baudrate
void init_spi(SDCard *sd, uint32_t baudrate) {
    spi_init(sd->spi, baudrate);
    spi_set_format(sd->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

// Initialize SD card
void init_card(SDCard *sd, uint32_t baudrate) {
    // Initialize CS pin
    gpio_init(sd->cs_pin);
    gpio_set_dir(sd->cs_pin, GPIO_OUT);
    gpio_put(sd->cs_pin, 1);

    // Initialize SPI at low baudrate for initialization
    init_spi(sd, 100000);

    // Clock card at least 100 cycles with CS high
    uint8_t dummy = 0xFF;
    for (int i = 0; i < 16; i++) {
        spi_write_blocking(sd->spi, &dummy, 1);
    }

    // CMD0: Initialize card (allow 5 attempts)
    for (int i = 0; i < 5; i++) {
        if (cmd(sd, 0, 0, 0x95, 0, true, false) == R1_IDLE_STATE) {
            break;
        }
    }
    if (cmd(sd, 0, 0, 0x95, 0, true, false) != R1_IDLE_STATE) {
        printf("Error: No SD card detected\n");
        return;
    }

    // CMD8: Determine card version
    int r = cmd(sd, 8, 0x01AA, 0x87, 4, true, false);
    if (r == R1_IDLE_STATE) {
        init_card_v2(sd);
    } else if (r == (R1_IDLE_STATE | R1_ILLEGAL_COMMAND)) {
        init_card_v1(sd);
    } else {
        printf("Error: Couldn't determine SD card version\n");
        return;
    }

    // Get number of sectors (CMD9)
    uint8_t csd[16];
    if (cmd(sd, 9, 0, 0, 0, false, false) != 0) {
        printf("Error: No response from SD card (CMD9)\n");
        return;
    }
    
    readinto(sd, csd, 16);

    if ((csd[0] & 0xC0) == 0x40) { // CSD version 2.0
        sd->sectors = (((csd[8] << 8) | csd[9]) + 1) * 1024;
    } else if ((csd[0] & 0xC0) == 0x00) { // CSD version 1.0
        uint32_t c_size = (csd[6] & 0b11) | (csd[7] << 2) | ((csd[8] & 0b11000000) << 4);
        uint32_t c_size_mult = ((csd[9] & 0b11) << 1) | (csd[10] >> 7);
        sd->sectors = (c_size + 1) * (1 << (c_size_mult + 2));
    } else {
        printf("Error: SD card CSD format not supported\n");
        return;
    }

    // CMD16: Set block length to 512 bytes
    if (cmd(sd, 16, 512, 0, 0, true, false) != 0) {
        printf("Error: Can't set 512 block size\n");
        return;
    }

    // Set to high data rate
    init_spi(sd, baudrate);
}

// Initialize SD card version 1
void init_card_v1(SDCard *sd) {
    for (int i = 0; i < CMD_TIMEOUT; i++) {
        cmd(sd, 55, 0, 0, 0, true, false);
        if (cmd(sd, 41, 0, 0, 0, true, false) == 0) {
            sd->cdv = 512;
            printf("SDCard: v1 card initialized\n");
            return;
        }
    }
    printf("Error: Timeout waiting for v1 card\n");
}

// Initialize SD card version 2
void init_card_v2(SDCard *sd) {
    for (int i = 0; i < CMD_TIMEOUT; i++) {
        sleep_ms(50);
        cmd(sd, 58, 0, 0, 4, true, false);
        cmd(sd, 55, 0, 0, 0, true, false);
        if (cmd(sd, 41, 0x40000000, 0, 0, true, false) == 0) {
            cmd(sd, 58, 0, 0, 4, true, false);
            sd->cdv = 1;
            printf("SDCard: v2 card initialized\n");
            return;
        }
    }
    printf("Error: Timeout waiting for v2 card\n");
}

// Send SD card command
int cmd(SDCard *sd, uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t final, bool release, bool skip1) {
    gpio_put(sd->cs_pin, 0);

    // Create and send command
    uint8_t buf[6];
    buf[0] = 0x40 | cmd;
    buf[1] = (arg >> 24) & 0xFF;
    buf[2] = (arg >> 16) & 0xFF;
    buf[3] = (arg >> 8) & 0xFF;
    buf[4] = arg & 0xFF;
    buf[5] = crc;
    spi_write_blocking(sd->spi, buf, 6);

    if (skip1) {
        uint8_t dummy = 0xFF;
        spi_write_read_blocking(sd->spi, &dummy, buf, 1);
    }

    // Wait for response
    uint8_t response;
    for (int i = 0; i < CMD_TIMEOUT; i++) {
        spi_write_read_blocking(sd->spi, (uint8_t[]){0xFF}, &response, 1);
        if (!(response & 0x80)) {
            for (int j = 0; j < final; j++) {
                spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
            }
            if (release) {
                gpio_put(sd->cs_pin, 1);
                spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
            }
            return response;
        }
    }

    // Timeout
    gpio_put(sd->cs_pin, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
    return -1;
}

// Read data into buffer
void readinto(SDCard *sd, uint8_t *buf, uint32_t len) {
    gpio_put(sd->cs_pin, 0);

    // Wait for start byte (0xFE)
    uint8_t token;
    for (int i = 0; i < CMD_TIMEOUT; i++) {
        spi_write_read_blocking(sd->spi, (uint8_t[]){0xFF}, &token, 1);
        if (token == TOKEN_DATA) {
            break;
        }
        sleep_us(100); // 0.0001 seconds
    }
    if (token != TOKEN_DATA) {
        gpio_put(sd->cs_pin, 1);
        printf("Error: Timeout waiting for response\n");
        return;
    }

    // Read data
    uint8_t dummy[512];
    memset(dummy, 0xFF, 512);
    spi_write_read_blocking(sd->spi, dummy, buf, len);

    // Read checksum
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);

    gpio_put(sd->cs_pin, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
}

// Write data with token
void write(SDCard *sd, uint8_t token, const uint8_t *buf, uint32_t len) {
    gpio_put(sd->cs_pin, 0);

    // Send start token, data, and checksum
    spi_write_blocking(sd->spi, &token, 1);
    spi_write_blocking(sd->spi, buf, len);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);

    // Check response
    uint8_t response;
    spi_write_read_blocking(sd->spi, (uint8_t[]){0xFF}, &response, 1);
    if ((response & 0x1F) != 0x05) {
        gpio_put(sd->cs_pin, 1);
        spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
        return;
    }

    // Wait for write to finish
    while (true) {
        spi_write_read_blocking(sd->spi, (uint8_t[]){0xFF}, &response, 1);
        if (response != 0) {
            break;
        }
    }

    gpio_put(sd->cs_pin, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
}

// Write token
void write_token(SDCard *sd, uint8_t token) {
    gpio_put(sd->cs_pin, 0);
    spi_write_blocking(sd->spi, &token, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);

    // Wait for write to finish
    uint8_t response;
    while (true) {
        spi_write_read_blocking(sd->spi, (uint8_t[]){0xFF}, &response, 1);
        if (response != 0x00) {
            break;
        }
    }

    gpio_put(sd->cs_pin, 1);
    spi_write_blocking(sd->spi, (uint8_t[]){0xFF}, 1);
}

// Read blocks
void readblocks(SDCard *sd, uint32_t block_num, uint8_t *buf, uint32_t len) {
    uint32_t nblocks = len / 512;
    if (len % 512 != 0 || nblocks == 0) {
        printf("Error: Invalid buffer length\n");
        return;
    }

    if (nblocks == 1) {
        // CMD17: Single block read
        if (cmd(sd, 17, block_num * sd->cdv, 0, 0, false, false) != 0) {
            gpio_put(sd->cs_pin, 1);
            printf("Error: CMD17 failed\n");
            return;
        }
        readinto(sd, buf, 512);
    } else {
        // CMD18: Multiple block read
        if (cmd(sd, 18, block_num * sd->cdv, 0, 0, false, false) != 0) {
            gpio_put(sd->cs_pin, 1);
            printf("Error: CMD18 failed\n");
            return;
        }
        for (uint32_t offset = 0; nblocks > 0; offset += 512, nblocks--) {
            readinto(sd, buf + offset, 512);
        }
        if (cmd(sd, 12, 0, 0xFF, 0, true, true) != 0) {
            printf("Error: CMD12 failed\n");
            return;
        }
    }
}

// Write blocks
void writeblocks(SDCard *sd, uint32_t block_num, const uint8_t *buf, uint32_t len) {
    uint32_t nblocks = len / 512;
    if (len % 512 != 0 || nblocks == 0) {
        printf("Error: Invalid buffer length\n");
        return;
    }

    if (nblocks == 1) {
        // CMD24: Single block write
        if (cmd(sd, 24, block_num * sd->cdv, 0, 0, true, false) != 0) {
            printf("Error: CMD24 failed\n");
            return;
        }
        write(sd, TOKEN_DATA, buf, 512);
    } else {
        // CMD25: Multiple block write
        if (cmd(sd, 25, block_num * sd->cdv, 0, 0, true, false) != 0) {
            printf("Error: CMD25 failed\n");
            return;
        }
        for (uint32_t offset = 0; nblocks > 0; offset += 512, nblocks--) {
            write(sd, TOKEN_CMD25, buf + offset, 512);
        }
        write_token(sd, TOKEN_STOP_TRAN);
    }
}

// IOCTL to get number of sectors
uint32_t ioctl(SDCard *sd, uint8_t op) {
    if (op == 4) { // Get number of blocks
        return sd->sectors;
    }
    return 0;
}

// Initialize SDCard structure
void sdcard_init(SDCard *sd, spi_inst_t *spi, uint cs_pin, uint32_t baudrate) {
    sd->spi = spi;
    sd->cs_pin = cs_pin;
    sd->sectors = 0;
    sd->cdv = 0;
    init_card(sd, baudrate);
}