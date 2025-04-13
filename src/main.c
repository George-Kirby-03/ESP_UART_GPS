
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
    .baud_rate = 115200, // Baud rate
    .data_bits = UART_DATA_8_BITS, // Data bits
    .parity = UART_PARITY_DISABLE, // Parity
    .stop_bits = UART_STOP_BITS_1, // Stop bits
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS, // Flow control
    .rx_flow_ctrl_thresh = 122, // RX flow control threshold
};

void app_main() {
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config_gps)); // Install UART driver
    printf("hOWDY");
}