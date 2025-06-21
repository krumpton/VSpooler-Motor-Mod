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

        // receive messages from hardware.c
        if (xQueueReceive(hardware_event_queue, &ev, 0)) {
            switch (ev) {
                case EVENT_BTN_PRESS_INVALID:
                    led_pulse_single(LED_RED, PULSE_DURATION / 4);
                    break;
                default: break;
            }
        }

        // if the LED is in the middle of doing a single pulse, changing its
        // state will cause it to end early, so check first.
        if (led_state.pulse_mode != PULSE_ONCE) {
            uint32_t colour = LED_WHITE;
            pulse_mode_t pulse_mode = PULSE_NONE;
            int pulse_duration = PULSE_DURATION;

            switch (machine_state & (BITMASK_RUNOUT | BITMASK_RUNNING)) {
                case BITMASK_RUNOUT: // filament unloaded, not running
                    pulse_mode = PULSE_LOOP;
                    pulse_duration = PULSE_DURATION;
                    break;
                case BITMASK_RUNNING: // filament loaded, running
                    pulse_mode = PULSE_LOOP;
                    pulse_duration = PULSE_DURATION * 2;
                    break;
                default: 
                    pulse_mode = PULSE_NONE;
                    break;
            }

            if (!(machine_state & BITMASK_SWITCH)) {
                colour = LED_MAGENTA; // switch = OFF - magenta
            }
            else {
                colour = LED_ORANGE; // switch = ON - orange
            }

            if (machine_state & BITMASK_RUNNING) {
                colour = LED_AZURE; // motor running - blue
            }
            else if (machine_state & BITMASK_PAUSED) {
                colour = LED_INDIGO; // manually paused via button
            }
            else if (machine_state & BITMASK_FINISHED) {
                colour = LED_CHARTREUSE; // stopped due to runout sensor
                pulse_mode = PULSE_NONE;
            }

            led_set_pulse(pulse_mode, pulse_duration);
            led_set_colour(colour);
        }

        led_tick();
    }
}