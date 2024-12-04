

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <rom/ets_sys.h>

#define BLINK_GPIO 5
#define BLINK_GPIO_4 4


#define GPIO_OUTPUT_PIN_SEL_IN  (1ULL<<GPIO_NUM_0) 
#define GPIO_OUTPUT_PIN_SEL_OUT (1ULL<<GPIO_NUM_2)

typedef enum{
    FLAG_DEACTIVATE,
    FLAG_ACTIVATED,
}flag_status;

typedef enum{LED_OFF,
LED_BLINK_ON,
}led_blink;

flag_status buttom_flag;


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



    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_OUT;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


}



void read_buttom(){
    while (1)
    {
        if (gpio_get_level(GPIO_NUM_0)==0){
            buttom_flag = FLAG_ACTIVATED;
            while (gpio_get_level(GPIO_NUM_0)==0){
                    vTaskDelay(10);
            }
        }
     vTaskDelay(10);
    }
}

void change_led_state(){
    led_blink status_led;
    status_led = LED_OFF;
    int blink_control=0; //variable to control the blink state
    while (1)
    {
        
        if(buttom_flag==FLAG_ACTIVATED){
            buttom_flag = FLAG_DEACTIVATE;
            if (status_led == LED_BLINK_ON) { //if the blink is activated turn off the led
                status_led = LED_OFF; //reset the led status
                gpio_set_level(GPIO_NUM_2, 0); //set the led gpio level to 0
                blink_control = 1; //chage the blink control to 1 for stoping the blinking
            } else if (status_led == LED_OFF) { //if the led is off activate the blinking
                status_led = LED_BLINK_ON;//fllowing led status 
                blink_control = 0; // Start blinking
            }
        }

        if (status_led == LED_BLINK_ON && blink_control == 0) { //check the blinking control and the fled status
            gpio_set_level(GPIO_NUM_2, 1); //turn on the led
            vTaskDelay(100 );//wait
            gpio_set_level(GPIO_NUM_2, 0); //turn off the led
            vTaskDelay(100) ;
        }
        vTaskDelay(10);
    }
    

}
void app_main()
{
    // //mutex, semaphore
    // xMutex = xSemaphoreCreateMutex();
    // //task mannage
    gpio_config_func();
    
    xTaskCreatePinnedToCore(&read_buttom, "read buttom", 2048, NULL, 5, NULL,0);
    xTaskCreatePinnedToCore(&change_led_state, "change led state", 2048,NULL,5,NULL ,0);
    

}