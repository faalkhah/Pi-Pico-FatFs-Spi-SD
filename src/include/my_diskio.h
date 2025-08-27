#ifndef MY_DISKIO_H
#define MY_DISKIO_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "my_ffconf.h"    //before ff.h
#include "ff.h"             // FatFs library
#include "diskio.h"         // FatFs disk I/O
#include "my_sdcard.h"

// Global SDCard instance declaration
extern SDCard sd;

#endif