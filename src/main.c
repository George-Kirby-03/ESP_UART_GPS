
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"


//UART Configuration structure


const uart_port_t uart_num = UART_NUM_2; // UART port number
uart_config_t uart_config_gps = {
    .baud_rate = 9600, // Baud rate
    .data_bits = UART_DATA_8_BITS, // Data bits
    .parity = UART_PARITY_DISABLE, // Parity
    .stop_bits = UART_STOP_BITS_1, // Stop bits
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Flow control
    .rx_flow_ctrl_thresh = 0, // RX flow control threshold
};

void app_main() {
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config_gps)); // Install UART driver
    ESP_ERROR_CHECK(uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)); // Set UART pins
    const int uart_buffer_size = 200; // Buffer size
    QueueHandle_t uart_queue; // UART queue
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, 0, 10, &uart_queue, 0)); // Install UART driver
    uint8_t data[100]; // Buffer for received data
    uart_event_t event;
    while (1) {
        // Wait for a UART event to occur
        int len = uart_read_bytes(uart_num, data, sizeof(data), 10/ portTICK_PERIOD_MS);
                   if (len > 0) {
                       
                       data[len] = '\0'; // Null-terminate the string
                       printf("%s", data); // Log the received data
                   }
        if (xQueueReceive(uart_queue, &event, 0)) {
            switch (event.type) {
                case UART_DATA:
                    break;
                case UART_FIFO_OVF:
                ESP_LOGE("UART", "Error fifo is full!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                uart_flush(uart_num); // Flush the UART buffer
                    break;
                default: 
                    ESP_LOGI("UART", "Unhandled event type: %d", event.type);
                    break;        
            }
        }

    }
}