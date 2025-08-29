/* debug_led_status.c
Debugging by pi-picco LED
2025 (C)Jalil Faalkhah 
*/

#ifndef DEBUG_LED_STATUS_H
#define DEBUG_LED_STATUS_H

#include "pico/stdlib.h"

typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINK_SLOW,
    LED_BLINK_FAST
} led_mode_t;

void led_status_init(uint pin);
void led_status_set(led_mode_t mode);
void led_status_update(void);

#endif
