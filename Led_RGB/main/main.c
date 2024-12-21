#include <stdio.h>
#include "driver/gpio.h"
#include "LED_RGB.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <esp_system.h>
#include "sdkconfig.h"

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

#define GPIO_OUTPUT_PIN_SEL_IN  (1ULL<<GPIO_NUM_0) 

typedef enum{LED_OFF,
LED_BLINK_ON,
}led_blink;  
typedef enum{
    FLAG_DEACTIVATE,
    FLAG_ACTIVATED,
}flag_status;
//QueueHandle_t buffer;//Objeto de la cola

flag_status buttom_flag;
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



void read_buttom(){
    while (1)
    {
        if (gpio_get_level(GPIO_NUM_0)==0){
            buttom_flag = FLAG_ACTIVATED;
            //xQueueSend(buffer, &buttom_s, pdMS_TO_TICKS(0));
            while (gpio_get_level(GPIO_NUM_0)==0){
                    vTaskDelay(10);
            }
        }
     vTaskDelay(10);
    }
}

void toggle_RGB(){
    led_blink status_led;
    status_led = LED_BLINK_ON;

    int blink_control=0; //variable to control the blink state
    while (1)
    {
        
        //xQueueReceive(buffer, &rcv, pdMS_TO_TICKS(1000));
        if (buttom_flag==FLAG_ACTIVATED){
            buttom_flag = FLAG_DEACTIVATE;//reset the led status
            if (status_led== LED_BLINK_ON){
                RGB_set_color(&LED_RGB,0,0,0); //set the led gpio level to 0
                blink_control = 1; //chage the blink control to 1 for stoping the blinking

            } else if (status_led == LED_OFF) { //if the led is off activate the blinking
                status_led = LED_BLINK_ON;//fllowing led status 
                blink_control = 0; // Start blinking
            }
        }

        if (status_led == LED_BLINK_ON && blink_control == 0) { //check the blinking control and the fled status
            RGB_set_color(&LED_RGB,80,0,80); 
            vTaskDelay(100 );//wait
            RGB_set_color(&LED_RGB,0,0,0); 
            vTaskDelay(100) ;
        }
        vTaskDelay(10);
    }
    

}


void gpio_config_func(){
        //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_IN;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


}

void app_main(void)
{
    //Configures the RGB led
    RGB_conf(&LED_RGB);
    gpio_config_func();
    //buffer = xQueueCreate(10, sizeof(uint32_t));//crea la cola *buffer* con 10 slots de 4 Bytes
    xTaskCreatePinnedToCore(&read_buttom, "read buttom", 2048, NULL, 5, NULL,0);
    xTaskCreatePinnedToCore(&toggle_RGB, "change led state", 2048,NULL,5,NULL ,0);

}
