#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "LED_RGB.h"



/*Define the adc configuration parameters*/
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_6
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_6



/*Define the configuration parameters for RGB LED*/
#define    SPEED_MODE           LEDC_LOW_SPEED_MODE            
#define    DUTY_RESOLUTION      LEDC_TIMER_8_BIT  
#define    TIMER_NUM            LEDC_TIMER_0  
#define    FREQ_HZ              (4000)                 
#define    R_GPIO_NUM           (15)  
#define    R_CHANNEL            0         
#define    G_GPIO_NUM           (2)  
#define    G_CHANNEL            1
#define    B_GPIO_NUM           (4)  
#define    B_CHANNEL            2
#define GPIO_OUTPUT_PIN_SEL_IN  (1ULL<<GPIO_NUM_0) 


typedef enum{
        RED_CONF,
        GREEN_CONF,
        BLUE_CONF,
        SET_CONF,
}color_conf; 


/* Queue handles for configuration and ADC values */
QueueHandle_t color_to_conf;  // Queue for color configuration

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



/**
 * @brief Configure ADC and return its handle.
 * 
 * This function initializes the ADC unit with the specified settings and 
 * configures the channel to be used for ADC measurements.
 * 
 * @return adc_oneshot_unit_handle_t Handle to the ADC unit.
 */
adc_oneshot_unit_handle_t adc_config( void){
    adc_oneshot_unit_handle_t handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    // Initialize ADC unit
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));

    //-------------ADC Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, EXAMPLE_ADC1_CHAN0, &config));
    return handle;
}

/**
 * @brief Task to read button state and update the color configuration queue.
 */

void read_buttom(){
    color_conf color_conf =SET_CONF;
    while (1)
    {
        if (gpio_get_level(GPIO_NUM_0)==0){ //check if the buttom is pressed
            xQueueReceive(color_to_conf, &color_conf,pdMS_TO_TICKS(100)); //read the colorto configure
            if (color_conf==SET_CONF)   //check if the previous state was set the color
            {
                color_conf=RED_CONF;        //prepare to configure the red color
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0)); //send the new color state
            }
            else if (color_conf==RED_CONF) //check if the previous state was red color
            { 
                color_conf=GREEN_CONF;  //prepare to configure the green color
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0));//send the new color state
            }
            
            else if(color_conf==GREEN_CONF){//check if the previous state was green color
                color_conf=BLUE_CONF; //prepare to configure the blue color
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0));//send the new color state
            }

            else{
                color_conf=SET_CONF; //if the previous state was blue color, then set the combination RGB
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0));//send the new color state
            }

            while (gpio_get_level(GPIO_NUM_0)==0){
                    vTaskDelay(10); 
            }
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


/**
 * @brief Task to configure color based on ADC values and queue state.
 */

void conf_color(){

    adc_oneshot_unit_handle_t adc1_handle=adc_config();
    color_conf color_config;
    int adc_value[1][1];
    //initialize the color's intensities
    float RED     =0;
    float GREEN   =0;
    float BLUE    =0;
    while (1)
    {
         // Read ADC value
        adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_value[0][0]);
        //read the color from the queue
        xQueueReceive(color_to_conf,&color_config,pdMS_TO_TICKS(100));
        
        /*check what color has to be configured*/
        if (color_config==RED_CONF)
            {
                RED= (adc_value[0][0]*100)/4095; //transform raw value from adc to a intensity 
                RGB_set_color(&LED_RGB,RED,0,0); //set the color
                vTaskDelay(10 );//wait

                
            }
        else if (color_config==GREEN_CONF)
            {
                
                GREEN= (adc_value[0][0]*100)/4095; //transform raw value from adc to a intensity
                RGB_set_color(&LED_RGB,0,GREEN,0); //set the color
                vTaskDelay(10 );//wait

            }
            
        else if(color_config==BLUE_CONF)
            {
                
                BLUE= (adc_value[0][0]*100)/4095; //transform raw value from adc to a intensity
                RGB_set_color(&LED_RGB,0,0,BLUE); //set the color
                vTaskDelay(10);//wait

            }

        else{
            RGB_set_color(&LED_RGB,RED,GREEN,BLUE); //set the combined color
            vTaskDelay(100 );//wait

        }
        

    }
    

}




void app_main(void)
{
    //configure the led RGB
    RGB_conf( &LED_RGB );
    //configure the buttom as an input
    gpio_config_func();
    //creat the queue
    color_to_conf = xQueueCreate( 10 , sizeof(uint32_t));  //crea la cola *color_to_conf* con 10 slots de 4 Bytes
   

    //create the task for buttom reading and color configuration
    xTaskCreatePinnedToCore( &read_buttom , "read buttom" , 2048 , NULL, 5, NULL , 0 );
    xTaskCreatePinnedToCore( &conf_color , "change led state" , 2048 , NULL , 5 , NULL , 0 );
   
    
}