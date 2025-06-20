#include "led.h"

CRGB* led_buffer;
led_state_t led_state;

static void led_reset_state() {
    led_state = (led_state_t) {
        .brightness = 0,
        .colour = LED_MAGENTA,
        .pulse_mode = PULSE_LOOP,
        .pulse_dir = PULSE_INCR,
        .pulse_duration = PULSE_DURATION,
    };
}

esp_err_t led_init(gpio_num_t pin) {
    led_reset_state();
    return ws28xx_init(pin, WS2812B, 1, &led_buffer);
}

void led_set_colour(uint32_t colour) {
    led_state.colour = colour;
}

void led_set_pulse(pulse_mode_t mode, int duration) {
    led_state.pulse_mode = mode;
    led_state.pulse_duration = duration;
}

void led_pulse_single() {

}

void led_tick() {
    if (led_state.pulse_mode == PULSE_LOOP) {
        switch (led_state.pulse_dir) {
            case PULSE_DECR:
                if (led_state.brightness > 0) {
                    led_state.brightness--;
                }
                else {
                    led_state.pulse_dir = PULSE_INCR;
                }
                break;

            case PULSE_INCR:
                if (led_state.brightness < led_state.pulse_duration) {
                    led_state.brightness++;
                }
                else {
                    led_state.pulse_dir = PULSE_DECR;
                }
                break;

            default: break;
        }
    }
    else if (led_state.pulse_mode == PULSE_ONCE) {
        // single pulse logic
    }
    else {
        led_state.brightness = led_state.pulse_duration;
    }

    // sanity check to prevent dividing by 0
    int duration = (led_state.pulse_duration > 0) ? led_state.pulse_duration : 1;

    led_buffer[0].r = (((led_state.colour >> 16) & 0xFF) * led_state.brightness) / duration;
    led_buffer[0].g = (((led_state.colour >> 8)  & 0xFF) * led_state.brightness) / duration;
    led_buffer[0].b = (((led_state.colour >> 0)  & 0xFF) * led_state.brightness) / duration;
    ws28xx_update();
}