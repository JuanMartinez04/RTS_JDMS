#include "UART_LIB.h"




void UART_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}




int sendData( const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len); //write the data bytes in the uart
    return txBytes;
}



char *rx_data()
{
    // Allocate dynamic memory for received data (+1 for null terminator)
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1); 
    //REad uart
    const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 100 / portTICK_PERIOD_MS);

    if (rxBytes > 0) {
        data[rxBytes] = '\0'; // null terminator
        ESP_LOGI("UART_TEST", "Received: '%s'", data);
        return (char *)data; // return the data
    } else {
        free(data); //free the memory space
        return NULL; // return null to indicate no data was received
    }
  
}