#include "hardware.h"

QueueHandle_t input_event_queue;

void IRAM_ATTR event_isr_handler(void* arg) {
    gpio_num_t pin = (gpio_num_t)(uintptr_t)arg;
    input_event_t e;

    switch (pin) {
        case    BTN_PIN: e = EVENT_BTN_PRESS; break;
        case SWITCH_PIN: e = EVENT_SWITCH_TOGGLE; break;
        case RUNOUT_PIN: e = EVENT_RUNOUT_TRIGGER; break;
        default: return;
    }

    xQueueSendFromISR(input_event_queue, &e, NULL);
}

void hardware_init() {
    // set up event queue
    input_event_queue = xQueueCreate(10, sizeof(input_event_t));

    // motor pins - output, no ISR
    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL << STEP_PIN) | (1ULL << DIR_PIN) | (1ULL << EN_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_out);

    // button pin - input, ISR on neg edge
    gpio_config_t io_in_neg = {
        .pin_bit_mask = 1ULL << BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&io_in_neg);

    // switch & runout sensor - input, ISR on any edge
    gpio_config_t io_in_any = {
        .pin_bit_mask = (1ULL << SWITCH_PIN) | (1ULL << RUNOUT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&io_in_any);

    // add ISR handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_PIN, event_isr_handler, (void*)BTN_PIN);
    gpio_isr_handler_add(SWITCH_PIN, event_isr_handler, (void*)SWITCH_PIN);
    gpio_isr_handler_add(RUNOUT_PIN, event_isr_handler, (void*)RUNOUT_PIN);

    // init motor pins
    gpio_set_level(DIR_PIN, 0); // HIGH = clockwise
    gpio_set_level(EN_PIN, gpio_get_level(SWITCH_PIN));
}

uint8_t read_inputs() {
    return (gpio_get_level(BTN_PIN)    << BTN_BIT) |
           (gpio_get_level(SWITCH_PIN) << SWITCH_BIT) |
           (gpio_get_level(RUNOUT_PIN) << RUNOUT_BIT);
}

void motor_step() {
    gpio_set_level(STEP_PIN, 1);
    ets_delay_us(STEP_PULSE_WIDTH_US);
    gpio_set_level(STEP_PIN, 0);
    ets_delay_us(STEP_DELAY_US - STEP_PULSE_WIDTH_US);
}