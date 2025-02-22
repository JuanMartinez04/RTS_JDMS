/**
 * Application entry point.
 */

#include "nvs_flash.h"
#include "driver/gpio.h"

#include "wifi_app.h"
#include "rgb_led.h"
#include "http_server.h"
#include "UART_LIB.h"
#include "ntc_pot.h"



QueueHandle_t TEMP_Q;
QueueHandle_t TEMP_Q_UART;

QueueHandle_t GET_TEMP_STATUS;
QueueHandle_t VOL_Q;

static void configure_led(void)
{
    
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	rgb_led_pwm_init();
}


void app_main(void)
{
	TEMP_Q= xQueueCreate( 2 , sizeof(uint32_t));
	TEMP_Q_UART = xQueueCreate( 2 , sizeof(uint32_t));
	GET_TEMP_STATUS = xQueueCreate( 1 , sizeof(uint32_t));
	VOL_Q  = xQueueCreate( 2 , sizeof(uint32_t));

	
    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	configure_led();
	UART_init();
	// Start Wifi
	//xTaskCreatePinnedToCore( &read_ntc , "read ntc" , 2048 , NULL, 5, NULL , 0 );
	
	xTaskCreatePinnedToCore( &GET_TEMP_FUNCTION , "uart ntc" , 4096 , NULL, 5, NULL , 1 );
	xTaskCreatePinnedToCore(&read_pot_ntc,"read ntc and pot", 4096 , NULL, 5, NULL , 1 );
	wifi_app_start();
	
}

