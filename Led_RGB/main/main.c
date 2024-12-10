#include <stdio.h>
#include "LED_RGB.h"
#include "freertos/FreeRTOS.h"

/*Define the configuration parameters for RGB LED*/
#define    SPEED_MODE LEDC_LOW_SPEED_MODE            
#define    DUTY_RESOLUTION   LEDC_TIMER_8_BIT  
#define    TIMER_NUM             LEDC_TIMER_0  
#define    FREQ_HZ           (4000)                 
#define    R_GPIO_NUM (15)  
#define    R_CHANNEL 0         
#define    G_GPIO_NUM (2)  
#define    G_CHANNEL 1
#define    B_GPIO_NUM (4)  
#define    B_CHANNEL 2


/*Initializes the rgb led configuration structure*/
led_RGB LED_RGB ={
    .speed_mode=        SPEED_MODE,                
    .duty_resolution=   DUTY_RESOLUTION ,     
    .timer_num=         TIMER_NUM,
    .freq_hz=           FREQ_HZ,                      
         
    .R_gpio_num=        R_GPIO_NUM,  
    .R_channel=         R_CHANNEL,         
    
    .G_gpio_num=        G_GPIO_NUM,  
    .G_channel=         G_CHANNEL,

    .B_gpio_num=        B_GPIO_NUM,  
    .B_channel=         B_CHANNEL
};






void app_main(void)
{
    //Configures the RGB led
    RGB_conf(&LED_RGB);
    while(1){
      //Set a mixed color
      RGB_set_color(&LED_RGB, 50.0, 50, 0);
      vTaskDelay(100); //1s delay
      RGB_set_color(&LED_RGB, 80, 0, 80.0); //set a purple color
      vTaskDelay(100);//1s delay
    }

}
