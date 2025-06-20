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
                case EVENT_BTN_PRESS:
                    break;
                case EVENT_RUNOUT_TRIGGER:
                    break;
                default: break;
            }
        }

        led_update(machine_state);
        led_tick(&led_state);
    }
}