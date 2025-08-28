/* my_diskio.c
Low level disk I/O module
2025 (C)Jalil Faalkhah 
*/

#include "my_diskio.h"

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