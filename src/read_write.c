    #include "read_write.h"

    // void write_file(){
    //     // Create and write to file
    //     fr = f_open(&fil, FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    //     if (fr == FR_OK) {
    //         const char *content = "Testing microSD Card \n";
    //         UINT bw;
    //         fr = f_write(&fil, content, strlen(content), &bw);
    //         if (fr != FR_OK || bw != strlen(content)) {
    //             printf("Error: Failed to write to %s (error %d)\n", FILE_PATH, fr);
    //         }
    //         f_close(&fil);
    //     } else {
    //         printf("Error: Failed to create %s (error %d)\n", FILE_PATH, fr);
    //     }
    // }

    void write_file(const char *path){
        // Create and write to file
        fr = f_open(&fil, path, FA_CREATE_ALWAYS | FA_WRITE);
        if (fr == FR_OK) {
            const char *content = "Testing microSD Card \n";
            UINT bw;
            fr = f_write(&fil, content, strlen(content), &bw);
            if (fr != FR_OK || bw != strlen(content)) {
                printf("Error: Failed to write to %s (error %d)\n", FILE_PATH, fr);
            }
            f_close(&fil);
        } else {
            printf("Error: Failed to create %s (error %d)\n", FILE_PATH, fr);
        }
    }

    void read_file(const char *path){
        // Read file contents
        // fr = f_open(&fil, FILE_PATH, FA_READ);
        fr = f_open(&fil, path, FA_READ);
        if (fr == FR_OK) {
            char buffer[64];
            UINT br;
            fr = f_read(&fil, buffer, sizeof(buffer) - 1, &br);
            if (fr == FR_OK) {
                buffer[br] = '\0'; // Null-terminate the string
                printf("File content: %s\n", buffer);
            } else {
                printf("Error: Failed to read %s (error %d)\n", FILE_PATH, fr);
            }
            f_close(&fil);
        } else {
            printf("Error: Failed to open %s for reading (error %d)\n", FILE_PATH, fr);
        }
    }
