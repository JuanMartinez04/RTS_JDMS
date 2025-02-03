#ifndef LED_RGB_H
#define LED_RGB_H

#include "driver/ledc.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

/**
 * @brief Configuration parameters of LEDRGB channels and timer for LED RGB_config function
 */
typedef struct{
    ledc_mode_t speed_mode;                /*!< LEDC speed speed_mode, high-speed mode (only exists on esp32) or low-speed mode */
    ledc_timer_bit_t duty_resolution;      /*!< LEDC channel duty resolution */
    ledc_timer_t  timer_num;               /*!< The timer source of channel (0 - LEDC_TIMER_MAX-1) */
    uint32_t freq_hz;                      /*!< LEDC timer frequency (Hz) */
   // ledc_clk_cfg_t clk_cfg;  
    //ledc_intr_type_t intr_type;    

    int R_gpio_num;                 /*!< the RED output gpio_num*/
    ledc_channel_t R_channel;       /*!< LEDRGB red channel (0) */
    
    int G_gpio_num;                 /*!< the GREEN output gpio_num*/
    ledc_channel_t G_channel;       /*!< LEDRGB GREEN channel (1) */

    int B_gpio_num;                 /*!< the BLUE output gpio_num*/
    ledc_channel_t B_channel;       /*!< LEDRGB BLUE channel (2) */


}led_RGB;


/**
 * @brief  Initialize and configure the RGB LED
 * This function sets up the LEDC timer and channels based on the provided
 * configuration
 *
 * @param led_RGB Pointer of LEDRGB configure struct
 *
 */
void RGB_conf(led_RGB *led_RGB);

/**
 * @brief Set and update the color of the RGB LED.
 * 
 * This function updates the duty cycle for each channel (Red, Green, Blue) based on
 * the provided intensity percentages
 * @param led_RGB Pointer of LEDRGB configure struct
 * @param R       Float value representing the intensity of the red channel (0.0 to 100.0).
 * @param G       Float value representing the intensity of the green channel (0.0 to 100.0).
 * @param B       Float value representing the intensity of the blue channel (0.0 to 100.0).
 *
 */
void RGB_set_color(led_RGB *led_RGB, float R, float G, float B);



#endif // LEDC_DRIVER_H