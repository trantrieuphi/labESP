#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_freertos_hooks.h"

volatile uint32_t ulIdleCycleCount = 0UL;
volatile uint32_t task1Counter = 0UL;
volatile uint32_t task2Counter = 0UL;
volatile uint32_t task3Counter = 0UL;

void vApplicationTickHook(void);

bool ApplicationIdleHook(void)
{
    ulIdleCycleCount++;
    return true;
}


void task_1(void *pvParameter)
{
    printf("----------task 1 start----------\n");
    int64_t start = esp_timer_get_time();
    while (1)
    {
        task1Counter++;
        if (!configUSE_PREEMPTION && task1Counter == 1000000)
        {
            vTaskDelay(0);
        }
        if (task1Counter >= 5000000)
        {
            int64_t end = esp_timer_get_time();
            printf("     task 1 during: %lld ms\n", (end - start) / 1000);
            printf("----------task 1 end------------\n");
            vTaskDelete(NULL);
        }
    }
}

void task_2(void *pvParameter)
{
    printf("----------task 2 start----------\n");
    int64_t start = esp_timer_get_time();
    while (1)
    {
        task2Counter++;
        if (task2Counter >= 5000000)
        {
            int64_t end = esp_timer_get_time();
            printf("     task 2 during: %lld ms\n", (end - start) / 1000);
            printf("----------task 2 end------------\n");
            vTaskDelete(NULL);
        }
    }
}

void task_3(void *pvParameter)
{
    printf("----------task 3 start----------\n");
    int64_t start = esp_timer_get_time();
    while (1)
    {
        task3Counter++;
        if (task3Counter >= 1000000)
        {
            int64_t end = esp_timer_get_time();
            printf("     task 3 during: %lld ms\n", (end - start) / 1000);
            printf("----------task 3 end------------\n");
            vTaskDelete(NULL);
        }
    }
}

void task_monitor(void *pvParameter)
{
    int count = 0;
    while (count < 14000000)
    {
        if (!(count % 250000))
        {
            printf("--------------------------------\n");
            printf("Task 1: %d \n", task1Counter);
            printf("Task 2: %d \n", task2Counter);
            printf("Task 3: %d \n", task3Counter);
            printf("Idle Count: %d \n", ulIdleCycleCount);
            printf("--------------------------------\n");
        }
        count++;
    }
    vTaskDelete(NULL);
}

void app_main()
{
    printf("Configuration: \n");
    printf("configUSE_PREEMPTION :   %d\n", configUSE_PREEMPTION);
    printf("configUSE_TIME_SLICING : %d\n", configUSE_TIME_SLICING);
    printf("----------------------------------------------------\n");

    if (configUSE_PREEMPTION && configUSE_TIME_SLICING)
    {
        printf("Pre-emptive Scheduling with Time Slicing\n");
    }
    else if (configUSE_PREEMPTION && !configUSE_TIME_SLICING)
    {
        printf("Pre-emptive Scheduling without Time Slicing\n");
    }
    else
    {
        printf("Co-operative Scheduling\n");
    }
    printf("----------------------------------------------------\n");
    esp_register_freertos_idle_hook(&ApplicationIdleHook);
    xTaskCreatePinnedToCore(&task_monitor, "task_monitor", 1024 * 5, NULL, 5, NULL, 1);

    if (configUSE_PREEMPTION)
    {
        xTaskCreatePinnedToCore(&task_1, "task 1", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL, 0);
        xTaskCreatePinnedToCore(&task_2, "task 2", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
        xTaskCreatePinnedToCore(&task_3, "task 3", 1024 * 5, NULL, 5, NULL, 0);
    }
    else
    {
        xTaskCreatePinnedToCore(&task_1, "task 1", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL, 0);
        xTaskCreatePinnedToCore(&task_2, "task 2", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(150));
        xTaskCreatePinnedToCore(&task_3, "task 3", 1024 * 5, NULL, 5, NULL, 0);
    }
}