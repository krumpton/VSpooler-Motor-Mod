#include "led.h"
#include "hardware.h"

void app_main(void) {
    // init LED buffer
    ESP_ERROR_CHECK_WITHOUT_ABORT(led_init(LED_PIN));
    // init GPIO & event queue
    hardware_init();

    while (1) {
        input_event_t ev;
        if (xQueueReceive(input_event_queue, &ev, portMAX_DELAY)) {
            switch (ev) {
                case EVENT_BTN_PRESS:
                case EVENT_SWITCH_TOGGLE:
                case EVENT_RUNOUT_TRIGGER:
                default: return;
            }
        }
        led_update(&led_state);
        ets_delay_us(200);
    }
}