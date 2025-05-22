#include "led.h"

const rgb_t LED_WHITE  = {255, 255, 255};
const rgb_t LED_RED    = {255, 0, 0};
const rgb_t LED_GREEN  = {0, 255, 0};
const rgb_t LED_BLUE   = {0, 0, 255};
const rgb_t LED_YELLOW = {255, 255, 0};
const rgb_t LED_PURPLE = {255, 0, 255};

CRGB* led_buffer;
led_state_t led_state;

static void led_reset_state() {
    led_state = (led_state_t) {
        .brightness = MAX_BRIGHTNESS,
        .colour = LED_PURPLE,
        .pulse = true,
        .pulse_dir = PULSE_DECR,
    };
}

esp_err_t led_init(gpio_num_t pin) {
    led_reset_state();
    return ws28xx_init(pin, WS2812B, 1, &led_buffer);
}

void led_update(led_state_t* state) {
    if (state->pulse) {
        switch (state->pulse_dir) {
            case PULSE_DECR:
                if (state->brightness > 0) {
                    state->brightness--;
                }
                else {
                    state->pulse_dir = PULSE_INCR;
                }
                break;

            case PULSE_INCR:
                if (state->brightness < MAX_BRIGHTNESS) {
                    state->brightness++;
                }
                else {
                    state->pulse_dir = PULSE_DECR;
                }
                break;

            default: return;
        }
    }
    else {
        state->brightness = MAX_BRIGHTNESS;
    }

    led_buffer[0].r = (state->colour.r * state->brightness) / MAX_BRIGHTNESS;
    led_buffer[0].g = (state->colour.g * state->brightness) / MAX_BRIGHTNESS;
    led_buffer[0].b = (state->colour.b * state->brightness) / MAX_BRIGHTNESS;
    ws28xx_update();
}