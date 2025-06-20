#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "esp_ws28xx.h"

#define MAX_BRIGHTNESS 4096

// typedefs
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

typedef enum {
    PULSE_NONE,
    PULSE_LOOP,
    PULSE_ONCE,
} pulse_mode_t;

typedef enum {
    PULSE_INCR,
    PULSE_DECR
} pulse_dir_t;

typedef struct {
    int brightness;
    rgb_t colour;
    pulse_mode_t pulse_mode;
    pulse_dir_t pulse_dir;
} led_state_t;

// LED pin
#define LED_PIN GPIO_NUM_2

// colours
extern const rgb_t LED_WHITE;
extern const rgb_t LED_RED;
extern const rgb_t LED_GREEN;
extern const rgb_t LED_BLUE;
extern const rgb_t LED_YELLOW;
extern const rgb_t LED_PURPLE;

// functions
esp_err_t led_init(gpio_num_t pin);
void led_update(uint8_t bitfield);
void led_tick(led_state_t* state);

// vars
extern CRGB* led_buffer;
extern led_state_t led_state;