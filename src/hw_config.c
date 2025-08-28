/* hw_config.h
Set gpio pins
2025 (C)Jalil Faalkhah
*/

#include "hw_config.h"

// Initialize SPI pins
void hw_init(){

    // Initialize SPI pins
    spi_init(SPI_BUS, 100000); // Initial low baudrate
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);

}
