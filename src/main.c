#include "hardware.h"
#include "led.h"

QueueHandle_t hardware_event_queue;

void app_main(void) {
    // create task queue
    hardware_event_queue = xQueueCreate(10, sizeof(hardware_event_t));
    // init LED buffer
    ESP_ERROR_CHECK_WITHOUT_ABORT(led_init(LED_PIN));
    
    // init GPIO & event queue
    hardware_init();

    while (1) {
        hardware_event_t ev;
        hardware_tick();

        if (xQueueReceive(hardware_event_queue, &ev, 0)) {
            switch (ev) {
                case EVENT_BTN_PRESS_VALID:
                    break;
                case EVENT_BTN_PRESS_INVALID:
                    break;
                default: break;
            }
        }

        if (led_state.pulse_mode != PULSE_ONCE) {
            uint32_t colour = LED_WHITE;
            pulse_mode_t pulse_mode = PULSE_NONE;
            int pulse_duration = 4096;

            switch (machine_state & (BITMASK_RUNOUT | BITMASK_RUNNING)) {
                case BITMASK_RUNOUT: // filament unloaded, not running
                    pulse_mode = PULSE_LOOP;
                    pulse_duration = 4096;
                    break;
                case BITMASK_RUNNING: // filament loaded, running
                    pulse_mode = PULSE_LOOP;
                    pulse_duration = 8192;
                    break;
                default: 
                    pulse_mode = PULSE_NONE;
                    break;
            }

            if (!(machine_state & BITMASK_SWITCH)) {
                colour = LED_CYAN; // switch = OFF - blue
            }
            else {
                colour = LED_ORANGE; // switch = ON - orange
            }

            led_set_pulse(pulse_mode, pulse_duration);
            led_set_colour(colour);
        }

        led_tick();
    }
}