#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"

// events
typedef enum {
    EVENT_BTN_PRESS,
    EVENT_RUNOUT_TRIGGER,
} input_event_t;

// machine state bitmasks
#define BITMASK_SWITCH   (1 << 0)
#define BITMASK_RUNOUT   (1 << 1)
#define BITMASK_RUNNING  (1 << 2)
#define BITMASK_STOPPING (1 << 3)
#define BITMASK_PAUSED   (1 << 4)
// bits 5 and 6 are unused
#define BITMASK_FINISHED (1 << 7)


// external event queue & state bitfield
extern QueueHandle_t hardware_event_queue;
extern uint8_t machine_state;

// functions
void hardware_init();
void hardware_tick();