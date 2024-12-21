#include "LED_RGB.h"



void RGB_conf(led_RGB *led_RGB){
    /*Prepare and then apply the LEDRGB PWM timer configuration*/
    ledc_timer_config_t led_RGB_timer = {
        .speed_mode       = led_RGB->speed_mode,
        .duty_resolution  = led_RGB->duty_resolution,
        .timer_num        = led_RGB->timer_num,
        .freq_hz          = led_RGB->freq_hz,  // Set output frequency in kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&led_RGB_timer));//reboot the program

    /*Configure the RED channel*/
    ledc_channel_config_t ledR_channel = {
        .speed_mode     = led_RGB->speed_mode, 
        .channel        = led_RGB->R_channel,
        .timer_sel      = led_RGB->timer_num,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = led_RGB->R_gpio_num,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledR_channel));
    /*Configure the GREEN channel*/
    ledc_channel_config_t ledG_channel = {
        .speed_mode     = led_RGB->speed_mode, 
        .channel        = led_RGB->G_channel,
        .timer_sel      = led_RGB->timer_num,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = led_RGB->G_gpio_num,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledG_channel));
    /*Configure the BLUE channel*/
    ledc_channel_config_t ledB_channel = {
        .speed_mode     = led_RGB->speed_mode, 
        .channel        = led_RGB->B_channel,
        .timer_sel      = led_RGB->timer_num,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = led_RGB->B_gpio_num,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledB_channel));


}
void RGB_set_color(led_RGB *led_RGB,float R, float  G, float B){
        
        /*Convert percentage (0.0-100.0) to duty cycle (0-255 for 8-bit resolution) for each channel*/
        int duty_R=(R/100)*(1<<led_RGB->duty_resolution);
        int duty_G=(G/100)*(1<<led_RGB->duty_resolution);
        int duty_B=(B/100)*(1<<led_RGB->duty_resolution);
        /*Set and update the RED channel duty cycle*/
        ledc_set_duty(led_RGB->speed_mode, led_RGB->R_channel, duty_R);
        ledc_update_duty(led_RGB->speed_mode, led_RGB->R_channel);
        /*Set and update the GREEN channel duty cycle*/
        ledc_set_duty(led_RGB->speed_mode, led_RGB->G_channel, duty_G);
        ledc_update_duty(led_RGB->speed_mode, led_RGB->G_channel);
        /*Set and update the BLUE channel duty cycle*/
        ledc_set_duty(led_RGB->speed_mode, led_RGB->B_channel, duty_B);
        ledc_update_duty(led_RGB->speed_mode, led_RGB->B_channel);

        vTaskDelay(10);
       

}