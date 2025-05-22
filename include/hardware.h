#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

// events
typedef enum {
    EVENT_BTN_PRESS,
    EVENT_RUNOUT_TRIGGER,
    EVENT_SWITCH_TOGGLE,
} input_event_t;

// LED pin
#define LED_PIN GPIO_NUM_2

// TMC2209 setup
#define STEP_DELAY_US 100
#define STEP_PULSE_WIDTH_US 20

#define STEP_PIN GPIO_NUM_8
#define DIR_PIN GPIO_NUM_9
#define EN_PIN GPIO_NUM_10

// input pins
#define BTN_PIN GPIO_NUM_3
#define SWITCH_PIN GPIO_NUM_4
#define RUNOUT_PIN GPIO_NUM_20

// input bit positions
#define BTN_BIT 2
#define SWITCH_BIT 1
#define RUNOUT_BIT 0

// event queue
extern QueueHandle_t input_event_queue;

// functions
void hardware_init();
int read_inputs();
void motor_step();