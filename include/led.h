#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "esp_ws28xx.h"

#define PULSE_DURATION 4096

// typedefs
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
    uint32_t colour;
    pulse_mode_t pulse_mode;
    pulse_dir_t pulse_dir;
    int pulse_duration;
} led_state_t;

// LED pin
#define LED_PIN GPIO_NUM_2

// COLOURS
// misc
#define LED_WHITE       0x00FFFFFFU
// primary
#define LED_RED         0x00FF0000U
#define LED_GREEN       0x0000FF00U
#define LED_BLUE        0x000000FFU
// secondary
#define LED_YELLOW      0x00FFFF00U
#define LED_CYAN        0x0000FFFFU
#define LED_MAGENTA     0x00FF00FFU
 // tertiary
#define LED_ORANGE      0x00FF8000U
#define LED_CHARTREUSE  0x0080FF00U
#define LED_SPRING      0x0000FF80U
#define LED_AZURE       0x000080FFU
#define LED_INDIGO      0x008000FFU
#define LED_ROSE        0x00FF0080U

// functions
esp_err_t led_init(gpio_num_t pin);
void led_set_colour(uint32_t colour);
void led_set_pulse(pulse_mode_t mode, int duration);
void led_pulse_single();
void led_tick();

// vars
extern CRGB* led_buffer;
extern led_state_t led_state;