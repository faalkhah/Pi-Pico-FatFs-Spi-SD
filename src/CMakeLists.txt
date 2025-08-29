# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_PATH "/home/carlk/pi/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(my_sd_card C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(../FatFs_SPI build)

# add_compile_definitions(
#     FF_CODE_PAGE=0
#     FF_USE_LFN=1
#     FF_MAX_LFN=255
#     FF_LFN_UNICODE=2
# )

include_directories(
    ${CMAKE_SOURCE_DIR}/include     #الویت اول
    ${CMAKE_SOURCE_DIR}/../FatFs_SPI/ff16/Source        #الویت دوم
)

# Add executable. Default name is the project name, version 1.0.0
add_executable(sd_card 
    main.c
    debug_led_status.c
    hw_config.c
    my_sdcard.c
    my_diskio.c
    dir.c
    read_write.c
)

# Add the standard library and FatFS/SPI to the build
target_link_libraries(
    sd_card
    pico_stdlib
    FatFs_SPI
    hardware_spi
)


pico_set_program_name(sd_card "sd_card")
pico_set_program_version(sd_card "1.0.1")

# Choose source and destination for standard input and output:
#   See 4.1. Serial input and output on Raspberry Pi Pico in Getting started with Raspberry Pi Pico (https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
#   and 2.7.1. Standard Input/Output (stdio) Support in Raspberry Pi Pico C/C++ SDK (https://datasheets.raspberrypi.org/pico/raspberry-pi-pico-c-sdk.pdf):
pico_enable_stdio_uart(sd_card 1)
pico_enable_stdio_usb(sd_card 1)

pico_add_extra_outputs(sd_card)

