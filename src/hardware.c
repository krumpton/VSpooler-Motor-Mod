#include "hardware.h"

typedef struct {
    uint32_t debounce_period_us;
    uint64_t last_triggered;
    gpio_num_t pin;
    bool check_level;
} debounce_info_t;

typedef enum {
    CLOCKWISE = 0,
    ANTICLOCKWISE = 1,
} motor_dir_t;

// TMC2209 setup
#define STEP_DELAY_US 100
#define STEP_PULSE_WIDTH_US 20

#define DIR_PIN GPIO_NUM_8
#define STEP_PIN GPIO_NUM_9
#define EN_PIN GPIO_NUM_10

// input pins
#define RUNOUT_PIN GPIO_NUM_1
#define BTN_PIN GPIO_NUM_3
#define SWITCH_PIN GPIO_NUM_4

static QueueHandle_t input_event_queue;
static debounce_info_t button_debounce = {
    .debounce_period_us = 150000,
    .last_triggered = 0,
    .pin = BTN_PIN,
    .check_level = true,
};
static debounce_info_t runout_debounce = {
    .debounce_period_us = 50000,
    .last_triggered = 0,
    .pin = RUNOUT_PIN,
    .check_level = false,
};

static int motor_speed = 0;
uint8_t machine_state;

// - ISR
void IRAM_ATTR event_isr_handler(void* arg) {
    gpio_num_t pin = (gpio_num_t)arg;
    hardware_event_t e;

    switch (pin) {
        case BTN_PIN: e = EVENT_BTN_PRESS;
            break;
        case RUNOUT_PIN: e = EVENT_RUNOUT_TRIGGER;
            break;
        default: return;
    }

    xQueueSendFromISR(input_event_queue, &e, NULL);
}

// - Static functions
static void set_bit(uint8_t* input, uint8_t bitmask, uint8_t state) {
    if (state) {
        *input |= bitmask;
    }
    else {
        *input &= ~bitmask;
    }
}

static void update_machine_state(uint8_t* state) {
    set_bit(&machine_state, BITMASK_SWITCH, !gpio_get_level(SWITCH_PIN));
    set_bit(&machine_state, BITMASK_RUNOUT, !gpio_get_level(RUNOUT_PIN));
}

static void motor_step() {
    gpio_set_level(STEP_PIN, 1);
    ets_delay_us(STEP_PULSE_WIDTH_US);
    gpio_set_level(STEP_PIN, 0);
    ets_delay_us(STEP_DELAY_US - STEP_PULSE_WIDTH_US);
}

static bool debounce(debounce_info_t* db) {
    uint64_t now = esp_timer_get_time();
    if (now - db->last_triggered < db->debounce_period_us) {
        return false;
    }

    if (db->check_level) {
        if (gpio_get_level(db->pin) != 0) {
            return false;
        }
    }

    db->last_triggered = now;
    return true;
}

// - Shared functions
void hardware_init() {
    // init queue & state
    input_event_queue = xQueueCreate(10, sizeof(hardware_event_t));
    machine_state = 0b00000000;

    // motor pins - output, no ISR
    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL << STEP_PIN) | (1ULL << DIR_PIN) | (1ULL << EN_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_out);

    // switch pin - input, no ISR
    gpio_config_t io_in = {
        .pin_bit_mask = 1ULL << SWITCH_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_in);

    // button pin - input, ISR on neg edge
    gpio_config_t io_in_neg = {
        .pin_bit_mask = 1ULL << BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&io_in_neg);

    // runout sensor - input, ISR on any edge
    gpio_config_t io_in_any = {
        .pin_bit_mask = 1ULL << RUNOUT_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&io_in_any);

    // add ISR handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_PIN, event_isr_handler, (void*)BTN_PIN);
    gpio_isr_handler_add(RUNOUT_PIN, event_isr_handler, (void*)RUNOUT_PIN);

    // init motor pins
    gpio_set_level(DIR_PIN, ANTICLOCKWISE);
    gpio_set_level(EN_PIN, gpio_get_level(SWITCH_PIN));
}

void hardware_tick() {
    // update machine state first to make sure button presses are 
    // validated against current state
    update_machine_state(&machine_state);

    hardware_event_t ev;
    if (xQueueReceive(input_event_queue, &ev, 0)) {
        switch (ev) {
            case EVENT_BTN_PRESS: 
                if (debounce(&button_debounce)) {
                    xQueueSend(hardware_event_queue, &ev, 0);
                }
            break;
            case EVENT_RUNOUT_TRIGGER: 
                if (debounce(&runout_debounce)) {
                    xQueueSend(hardware_event_queue, &ev, 0);
                }
            break;
            default: break;
        }
    }

    gpio_set_level(EN_PIN, gpio_get_level(SWITCH_PIN));

    if (machine_state & BITMASK_RUNNING) {
        motor_step();
    }
    else {
        ets_delay_us(STEP_DELAY_US);
    }
}