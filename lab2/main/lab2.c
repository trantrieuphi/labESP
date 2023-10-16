
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "led_strip.h"
#include "sdkconfig.h"
// #include "portmacro.h"


#define PUSH_BUTTON_PIN GPIO_NUM_13

static void configure_button(void)
{
    gpio_set_pull_mode(PUSH_BUTTON_PIN, GPIO_PULLUP_ONLY);
    gpio_set_level(PUSH_BUTTON_PIN, 0);
    gpio_set_direction(PUSH_BUTTON_PIN, GPIO_MODE_INPUT);
}

void app_main(void)
{
    configure_button();
    int current_state = 1;
    int last_state = 1;
    int count = 0;
    int flag = 0;
    while (1)
    {
        current_state = gpio_get_level(PUSH_BUTTON_PIN);

        if (last_state == current_state && current_state == 0)
        {
            if (flag == 0)
            {
                printf("ESP32\n");
            }
            flag++;
        }
        else
        {
            last_state = current_state;
            flag = 0;
        }

        if (count == 49)
        {
            printf("1914616, 1912600 \n");
            count = 0;
        }
        count++;
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}