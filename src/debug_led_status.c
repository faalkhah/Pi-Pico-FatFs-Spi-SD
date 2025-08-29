/* debug_led_status.c
Debugging by pi-picco LED
2025 (C)Jalil Faalkhah 
*/
#include "debug_led_status.h"

static uint led_pin;
static led_mode_t current_mode = LED_OFF;
static absolute_time_t last_toggle_time;
static bool led_state = false;

void led_status_init(uint pin) {
    led_pin = pin;
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_put(led_pin, 0);
    current_mode = LED_OFF;
    last_toggle_time = get_absolute_time();
}

void led_status_set(led_mode_t mode) {
    current_mode = mode;
}

void led_status_update(void) {
    if (current_mode == LED_OFF) {
        gpio_put(led_pin, 0);
        return;
    }
    if (current_mode == LED_ON) {
        gpio_put(led_pin, 1);
        return;
    }

    int interval_ms = (current_mode == LED_BLINK_SLOW) ? 1000 : 100; // slow or fast

    if (absolute_time_diff_us(last_toggle_time, get_absolute_time()) / 1000 >= interval_ms) {
        led_state = !led_state;
        gpio_put(led_pin, led_state);
        last_toggle_time = get_absolute_time();
    }
}
