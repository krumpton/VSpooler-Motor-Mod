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

static int motor_speed_pct = 0;
static int motor_speed_step = 5;
static int motor_speed_max = 1000;
uint8_t machine_state;

// - ISR
void IRAM_ATTR event_isr_handler(void* arg) {
    hardware_event_t e = EVENT_BTN_PRESS;
    xQueueSendFromISR(input_event_queue, &e, NULL);
}

// - Static functions
// set an individual bit in the bitfield
static void set_bit(uint8_t* input, uint8_t bitmask, uint8_t state) {
    if (state) {
        *input |= bitmask;
    } else {
        *input &= ~bitmask;
    }
}

// set switch and runout bits based on pin state
static void update_machine_state(uint8_t* state) {
    set_bit(&machine_state, BITMASK_SWITCH, !gpio_get_level(SWITCH_PIN));
    set_bit(&machine_state, BITMASK_RUNOUT, !gpio_get_level(RUNOUT_PIN));

    if ((machine_state & BITMASK_PAUSED) && 
    (!gpio_get_level(SWITCH_PIN) || !gpio_get_level(RUNOUT_PIN))) {
        set_bit(&machine_state, BITMASK_PAUSED, 0);
    }
}

// step the motor once & adjust speed as needed
static void motor_step() {
    if (machine_state & BITMASK_STOPPING) {
        if (motor_speed_pct > 0) {
            motor_speed_pct -= motor_speed_step;

            // speed percent shouldn't go below 0, but check to be safe
            if (motor_speed_pct <= 0) {
                motor_speed_pct = 0;
                set_bit(&machine_state, BITMASK_RUNNING, 0);
                set_bit(&machine_state, BITMASK_STOPPING, 0);
            }
        }
    } else { // stop bit not set
        if (motor_speed_pct < motor_speed_max) {
            motor_speed_pct += motor_speed_step;
        }
    }

    if (motor_speed_pct > motor_speed_max) {
        motor_speed_pct = motor_speed_max;
    }

    int speed_safe = (motor_speed_pct > 0) ? motor_speed_pct : 1;
    int delay = STEP_DELAY_US * (motor_speed_max / speed_safe);

    gpio_set_level(STEP_PIN, 1);
    ets_delay_us(STEP_PULSE_WIDTH_US);
    gpio_set_level(STEP_PIN, 0);
    ets_delay_us(delay - STEP_PULSE_WIDTH_US);
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

    // switch pin & runout pin - input, no ISR
    gpio_config_t io_in = {
        .pin_bit_mask = (1ULL << SWITCH_PIN) | (1ULL << RUNOUT_PIN),
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

    // add ISR handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_PIN, event_isr_handler, NULL);

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
                    if (!(machine_state & BITMASK_RUNNING)) {  // motor not running
                        // check if switch & runout are valid
                        if ((machine_state & BITMASK_SWITCH) && !(machine_state & BITMASK_RUNOUT)) {
                            set_bit(&machine_state, BITMASK_RUNNING, 1);

                            if (machine_state & BITMASK_PAUSED) {
                                set_bit(&machine_state, BITMASK_PAUSED, 0);
                            }
                        } else {
                            // send invalid event to task queue
                            ev = EVENT_BTN_PRESS_INVALID;
                            xQueueSend(hardware_event_queue, &ev, 0);
                        }
                    } else {
                        // set pause bit if manually stopped, for different LED state
                        set_bit(&machine_state, BITMASK_STOPPING, 1);
                        set_bit(&machine_state, BITMASK_PAUSED, 1);
                    }
                }
                break;
            default:
                break;
        }
    }

    // unset finished bit if the user turns off the switch
    if ((machine_state & BITMASK_FINISHED) && !(machine_state & BITMASK_SWITCH)) {
        set_bit(&machine_state, BITMASK_FINISHED, 0);
    }

    gpio_set_level(EN_PIN, gpio_get_level(SWITCH_PIN));

    if (machine_state & BITMASK_RUNNING) {
        if (machine_state & BITMASK_RUNOUT) {
            set_bit(&machine_state, BITMASK_STOPPING, 1);
            set_bit(&machine_state, BITMASK_FINISHED, 1);
        }
        motor_step();
    } else {
        ets_delay_us(STEP_DELAY_US);
    }
}