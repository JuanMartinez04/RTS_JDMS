#ifndef UART_LIB_H
#define UART_LIB_H
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)


/**
 * @brief Initializes the UART with the specified configuration.
 */

void UART_init(void);

/**
 * @brief Sends data over UART.
 * @param data Pointer to the string to be sent.
 * @return Number of bytes transmitted.
 */
int sendData( const char* data);

/**
 * @brief Receives data from UART.
 * @return Pointer to a string containing the received data (must be freed after use).
 *         Returns NULL if no data is received.
 */
char *rx_data();




#endif