
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

void esp_task_wdt_isr_user_handler(void){
    printf("WDT ISR handler\n");
}

//UART Configuration structure


uint8_t data[100]; // Buffer for received data
uart_event_t event;
QueueHandle_t uart_queue; // UART queue
const uart_port_t uart_num = UART_NUM_2; // UART port number

void init_uart() {
    
    uart_config_t uart_config_gps = {
    .baud_rate = 9600, // Baud rate
    .data_bits = UART_DATA_8_BITS, // Data bits
    .parity = UART_PARITY_DISABLE, // Parity
    .stop_bits = UART_STOP_BITS_1, // Stop bits
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Flow control
    .rx_flow_ctrl_thresh = 0, // RX flow control threshold
};
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config_gps)); // Install UART driver
    ESP_ERROR_CHECK(uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)); // Set UART pins
    const int uart_buffer_size = 200; // Buffer size
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, 0, 10, &uart_queue, 0)); // Install UART driver
}

static void uart_event_task(void *pvParameters) {
    while (1) {
        // Wait for a UART event to occur
        int len = uart_read_bytes(uart_num, data, sizeof(data), 1/ portTICK_PERIOD_MS);
        if (len > 0) {   
            
            data[len] = '\0'; // Null-terminate the string
            printf("%s", data); // Log the received data
        }
        else if (len == 0) {
            ESP_LOGW("UART", "No data");
        }
        else if (len < 0) {
            ESP_LOGE("UART", "Error reading data");
        };
        
        if (xQueueReceive(uart_queue, &event, 0)) {
            switch (event.type) {
                case UART_DATA:
                    break;
                default: 
                    ESP_LOGI("UART", "Unhandled event type: %d", event.type);
                    uart_flush(uart_num); // Flush the UART buffer
                    break;        
            }
        }
    }
}

TaskHandle_t taskAHandle;
TaskHandle_t taskBHandle;
// Perform an action every 10 ticks.
void vTaskFunction( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 10;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
   // printf("Hello refrefsedfgsegtrsdtgtrdgrtdgthr!\n");
    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        printf("Hello World tAK111111111! %lu\n", xLastWakeTime);
        // Perform action here.
    }
}

static void loop1(void *pvParameters) {
    while(taskBHandle == NULL){
        printf("Waiting for task B to be created...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    }
    while(1){
        printf("Hello World tAK222222222!\n");
        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for 1 second
        vTaskResume(taskBHandle);
        vTaskSuspend(NULL); // Suspend task A
    }
}

static void loop2(void *pvParameters) {
    while(taskAHandle == NULL){
        printf("Waiting for task A to be created...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    }
    // Wait for task A to be created
    while(1){
        printf("Hello World tAK2!\n");
        vTaskDelay(100 / portTICK_PERIOD_MS); 
        vTaskResume(taskAHandle); // Delay for 1 second
        vTaskSuspend(NULL); // Suspend task B
    }
}


void app_main() {
    // init_uart();
   //  xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, configMAX_PRIORITIES - 2, NULL); // Create UART event task
    xTaskCreatePinnedToCore(vTaskFunction, "loop", 2048, NULL, configMAX_PRIORITIES - 2, NULL, 0); // Create loop task
  
   xTaskCreatePinnedToCore(loop1, "loop1", 2048, NULL, configMAX_PRIORITIES - 3, &taskAHandle, 0); // Create loop task 
  
   xTaskCreatePinnedToCore(loop2, "loop2", 2048, NULL, configMAX_PRIORITIES - 3, &taskBHandle, 0); // Create loop task
   // esp_task_wdt_isr_user_handler();
    // esp_task_wdt_init(10, true); // Initialize WDT with a timeout of 10 seconds
    // esp_task_wdt_add(NULL); // Add the current task to WDT
    // esp_task_wdt_delete(NULL); // Delete the current task from WDT
}