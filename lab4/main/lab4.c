#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

QueueHandle_t xQueue;

typedef enum
{
    eMotorSpeed,
    eSpeedSetPoint,
    eAnother
} ID_t;

typedef struct
{
    ID_t eDataID;
    float value;
} Data_t;

float float_rand(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);       /* [min, max] */
}

/*-----------------------------------------------------------*/

static void veMotorSpeedSenderTask(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
    Data_t txBuffer;
    srand(time(0));

    /* As per most tasks, this task is implemented within an infinite loop. */
    for (;;)
    {
        txBuffer.eDataID = eMotorSpeed;
        txBuffer.value = float_rand(0, 10);

        xStatus = xQueueSendToBack(xQueue, &txBuffer, xTicksToWait);

        if (xStatus != pdPASS)
        {
            printf("Could not send to the queue.\r\n");
        }
        else
        {
            printf("Task eMotorSpeed is sent.\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void veSpeedSetPointSenderTask(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
    Data_t txBuffer;
    srand(time(0));

    /* As per most tasks, this task is implemented within an infinite loop. */
    for (;;)
    {
        txBuffer.eDataID = eSpeedSetPoint;
        txBuffer.value = float_rand(10, 20);

        xStatus = xQueueSendToBack(xQueue, &txBuffer, xTicksToWait);

        if (xStatus != pdPASS)
        {
            printf("Could not send to the queue.\r\n");
        }
        else
        {
            printf("Task eSpeedSetPoint is sent.\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void vAnotherTask(void *pvParameters)
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);
    Data_t txBuffer;
    srand(time(0));

    /* As per most tasks, this task is implemented within an infinite loop. */
    for (;;)
    {
        txBuffer.eDataID = eAnother;
        txBuffer.value = float_rand(10, 20);

        xStatus = xQueueSendToBack(xQueue, &txBuffer, xTicksToWait);

        if (xStatus != pdPASS)
        {
            printf("Could not send to the queue.\r\n");
        }
        else
        {
            printf("Task eAnother is sent.\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
/*-----------------------------------------------------------*/
static void vReceiverTask(void *pvParameters)
{
    Data_t xReceivedStructure;

    for (;;)
    {
        if (uxQueueMessagesWaiting(xQueue) == 3)
        {
            printf("Queue is full\r\n");
        }
        if (xQueueReceive(xQueue, &xReceivedStructure, 0) == pdPASS)
        {
            if (xReceivedStructure.eDataID == eMotorSpeed)
            {
                printf("Written data for eMotorSpeed = %f\n", xReceivedStructure.value);
            }
            else if (xReceivedStructure.eDataID == eSpeedSetPoint)
            {
                printf("Written data for eSpeedSetPoint = %f\n", xReceivedStructure.value);
            }
            else
            {
                printf("Ignore another task!\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{
    xQueue = xQueueCreate(3, sizeof(Data_t));

    if (xQueue != NULL)
    {
        xTaskCreatePinnedToCore(&veMotorSpeedSenderTask, "Sender", 1024 * 5, NULL, 3, NULL, 0);
        xTaskCreatePinnedToCore(&veSpeedSetPointSenderTask, "Sender", 1024 * 5, NULL, 2, NULL, 0);
        xTaskCreatePinnedToCore(&vAnotherTask, "Sender", 1024 * 5, NULL, 2, NULL, 0);
        xTaskCreatePinnedToCore(&vReceiverTask, "Receiver", 1024 * 5, NULL, 1, NULL, 0);
    }
    else
    {
        printf("The queue could not be created.\r\n");
    }
}