/* my_sdcard.h
Main file to interface spi, mount and detect SD version 
2025 (C)Jalil Faalkhah 
*/

#ifndef MY_SDCARD_H
#define MY_SDCARD_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Constants from MicroPython driver
#define CMD_TIMEOUT 1000
#define R1_IDLE_STATE (1 << 0)          //00000001
#define R1_ILLEGAL_COMMAND (1 << 2)     //00000100
#define TOKEN_CMD25 0xFC
#define TOKEN_STOP_TRAN 0xFD
#define TOKEN_DATA 0xFE

// SDCard structure to hold SPI and CS pin information
typedef struct {
    spi_inst_t *spi;
    uint cs_pin;
    uint32_t sectors;
    uint32_t cdv; // Card division factor (1 for SDHC/SDXC, 512 for SDSC)
} SDCard;

// Function prototypes
void init_spi(SDCard *sd, uint32_t baudrate);
void init_card(SDCard *sd, uint32_t baudrate);
void init_card_v1(SDCard *sd);
void init_card_v2(SDCard *sd);
int cmd(SDCard *sd, uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t final, bool release, bool skip1);
void readinto(SDCard *sd, uint8_t *buf, uint32_t len);
void write(SDCard *sd, uint8_t token, const uint8_t *buf, uint32_t len);
void write_token(SDCard *sd, uint8_t token);
uint32_t get_sectors(SDCard *sd);
void sdcard_init(SDCard *sd, spi_inst_t *spi, uint cs_pin, uint32_t baudrate);
void readblocks(SDCard *sd, uint32_t block_num, uint8_t *buf, uint32_t len);
void writeblocks(SDCard *sd, uint32_t block_num, const uint8_t *buf, uint32_t len);
uint32_t ioctl(SDCard *sd, uint8_t op);

#endif