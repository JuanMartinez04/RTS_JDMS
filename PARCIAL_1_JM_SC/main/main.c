#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"


#include "LED_RGB.h"
#include "ADC_LIB.h"
#include "UART_LIB.h"



/*Define the adc configuration parameters*/
#define ADC1_CHAN                  ADC_CHANNEL_6
#define ADC1_ATTEN                 ADC_ATTEN_DB_11


/*Define the adc configuration parameters*/
#define EXAMPLE_ADC2_CHAN4          ADC_CHANNEL_4
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_6

/*Define the configuration parameters for RGB LED1*/
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

/*Define the configuration parameters for RGB LED2*/

#define    SPEED_MODE2           LEDC_LOW_SPEED_MODE            
#define    DUTY_RESOLUTION2      LEDC_TIMER_8_BIT  
#define    TIMER_NUM2            LEDC_TIMER_1
#define    FREQ_HZ2              (4000)                 
#define    R_GPIO_NUM2           (12)  
#define    R_CHANNEL2            3         
#define    G_GPIO_NUM2           (14)  
#define    G_CHANNEL2            4
#define    B_GPIO_NUM2           (27)  
#define    B_CHANNEL2            5
#define GPIO_OUTPUT_PIN_SEL_IN2  (1ULL<<GPIO_NUM_0)


/* Define FreeRTOS queues to store temperature and RGB thresholds */
QueueHandle_t TEMP_Q;

QueueHandle_t R_MIN;
QueueHandle_t R_MAX;

QueueHandle_t G_MIN;
QueueHandle_t G_MAX;

QueueHandle_t B_MIN;
QueueHandle_t B_MAX;

QueueHandle_t GET_TEMP_STATUS;
QueueHandle_t color_to_conf;

//DEeine macros
typedef enum {
    RED_CONF2,
    GREEN_CONF2,
    BLUE_CONF2,
    SET_CONF2
} color_conf;

#define  T_ON (1)
#define T_OFF (0)
#define OFF (0)
#define MAX_BRIGHT (99)

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


led_RGB LED_RGB2 ={
    .speed_mode=        SPEED_MODE2,                
    .duty_resolution=   DUTY_RESOLUTION2 ,     
    .timer_num=         TIMER_NUM2,
    .freq_hz=           FREQ_HZ2,                      
         
    .R_gpio_num=        R_GPIO_NUM2,  
    .R_channel=         R_CHANNEL2,         
    
    .G_gpio_num=        G_GPIO_NUM2,  
    .G_channel=         G_CHANNEL2,

    .B_gpio_num=        B_GPIO_NUM2,  
    .B_channel=         B_CHANNEL2
};



/**
 * @brief NTC read function
 * 
 */

void read_ntc(){
    //create variables to store the raw and voltage values
    int adc_value[1][1];
    int voltage_value[1][1];
    int temperature;
    adc_oneshot_unit_handle_t adc1_handle=adc_config(ADC1_ATTEN,ADC1_CHAN); //conf adc1 chan 6
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    bool do_calibration1_chan0 = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN, ADC1_ATTEN, &adc1_cali_chan0_handle);//calibration
    for(;;){
        adc_oneshot_read(adc1_handle, ADC1_CHAN, &adc_value[0][0]);//read raw
        if(do_calibration1_chan0){
        adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_value[0][0], &voltage_value[0][0]);}//REad voltage
        temperature = (-0.033324*voltage_value[0][0])+78.22 ; //convert voltage to temperaure value in °c
        xQueueSend( TEMP_Q , &temperature , pdMS_TO_TICKS(0)); //send the temperature
        vTaskDelay(100);
    }

}
void GET_TEMP_FUNCTION(){
    int status;
    char message_to_send[RX_BUF_SIZE+1];
    int temp;
    while (1)
    {
       xQueueReceive(TEMP_Q,&temp, 100/ portTICK_PERIOD_MS);
       xQueueReceive(GET_TEMP_STATUS,&status,100/ portTICK_PERIOD_MS);
        if (status != NULL ){

            if(status == T_ON ){
                sprintf(message_to_send,"la temperatura es %d \n ",temp);
                sendData(message_to_send);
                vTaskDelay(pdMS_TO_TICKS(2000));}
        }
        vTaskDelay(10);   
    }
}

/**
 * @brief Splits an input string into three words
 * 
 * @param input Input string
 * @param words Array to store the three extracted words
 */
void str_to_chars(const char *input, char words[3][RX_BUF_SIZE+1]) {
    char copy[RX_BUF_SIZE+1]; //make a copy of the original string
    strncpy(copy, input, RX_BUF_SIZE);
    copy[RX_BUF_SIZE] = '\0';

    const char delimitador[] = " ";
    char *token = strtok(copy, delimitador);

    for (int i = 0; i < 3; i++) {
        if (token != NULL) {
            strncpy(words[i], token,RX_BUF_SIZE);
            words[i][RX_BUF_SIZE] = '\0';
            token = strtok(NULL, delimitador);
        } else {
            words[i][0] = '\0';
        }
    }
}
/**
 * @brief Handles UART commands to control the LED and get temperature data
 */
void UART_commands(){
    int temp;
    char message[RX_BUF_SIZE+1];
    while (1) {
        xQueueReceive(TEMP_Q , &temp , 100/ portTICK_PERIOD_MS);

        char *receive_c = rx_data(); //read uart 

        if (receive_c!= NULL){ //if string is not null
            char (*words)[RX_BUF_SIZE+1] = malloc(3 * 1025); 
            str_to_chars(receive_c,words);
            
            if (strcmp( words[0] , "GET_TEMP_ON") ==0 ) {
                int status_ = T_ON;
                sprintf(message, "the temperature is: %d",temp);
                sendData(message);
                xQueueSend(GET_TEMP_STATUS , &status_, pdMS_TO_TICKS(0));
 
            } 
            else if (strcmp(words[0],"GET_TEMP_OFF")==0) {
                int status_ = T_OFF;
                xQueueSend(GET_TEMP_STATUS , &status_ , pdMS_TO_TICKS(0));
            } 
            else if (strcmp(words[0],"RED")==0)
            {
                if (strcmp(words[1],"MIN")==0){
                    int red_min_limit;
                    red_min_limit=atoi(words[2]);
                    xQueueSend(R_MIN,&red_min_limit,pdMS_TO_TICKS(0));
                    free(words);
                }
                else if (strcmp(words[1],"MAX")==0){
                    int red_max_limit;
                    red_max_limit=atoi(words[2]);
                    xQueueSend(R_MAX,&red_max_limit,pdMS_TO_TICKS(0));
                    free(words);
                }
                else{
                        sendData("No valid command");
                        free(words);
                }
            }

            else if (strcmp(words[0],"GREEN")==0)
            {
                if (strcmp(words[1],"MIN")==0){
                    int green_min_limit;
                    green_min_limit=atoi(words[2]);
                    xQueueSend(G_MIN,&green_min_limit,pdMS_TO_TICKS(0));
                    free(words);
                }
                else if (strcmp(words[1],"MAX")==0){
                    int green_max_limit;
                    green_max_limit=atoi(words[2]);
                    xQueueSend(G_MAX,&green_max_limit,pdMS_TO_TICKS(0));
                    free(words);
                }
                else{
                        sendData("No valid command");
                        free(words);
                }
            }
            else if (strcmp(words[0],"BLUE")==0)
            {
                if (strcmp(words[1],"MIN")==0){
                    int blue_min_limit;
                    blue_min_limit=atoi(words[2]);
                    xQueueSend(B_MIN,&blue_min_limit,pdMS_TO_TICKS(0));
                    free(words);
                }
                else if (strcmp(words[1],"MAX")==0){
                    int blue_max_limit;
                    blue_max_limit=atoi(words[2]);
                    xQueueSend(B_MAX,&blue_max_limit,pdMS_TO_TICKS(0));
                    free(words);
                }
                else{
                        sendData("No valid command");
                        free(words);
                }
            }

            else {
                sendData("Thats not a valid command");
            }
        }
        
        vTaskDelay(10); // Pequeño retardo
    }
}
/**
 * @brief Configures the RGB LED color based on temperature thresholds
 */
void colorconf(){
    int temp_;

    int rmax_limit=OFF;
    int rmin_limit=OFF;

    int gmin_limit=OFF;
    int gmax_limit=OFF;

    int bmin_limit=OFF;
    int bmax_limit=OFF;

    int R;
    int G;
    int B;

    while (1)
    {
        /* code */
        xQueueReceive(TEMP_Q,&temp_, 100/ portTICK_PERIOD_MS);

        xQueueReceive(R_MIN,&rmin_limit,100/ portTICK_PERIOD_MS);
        xQueueReceive(R_MAX,&rmax_limit,100/ portTICK_PERIOD_MS);

        xQueueReceive(G_MIN,&gmin_limit,100/ portTICK_PERIOD_MS);
        xQueueReceive(G_MAX,&gmax_limit,100/ portTICK_PERIOD_MS);

        xQueueReceive(B_MIN,&bmin_limit,100/ portTICK_PERIOD_MS);
        xQueueReceive(B_MAX,&bmax_limit,100/ portTICK_PERIOD_MS);

        //ESP_LOGI("rmin","%d",rmin_limit);
        if (rmin_limit<temp_ && temp_<rmax_limit){
                R=MAX_BRIGHT;
             
        }
        if (gmin_limit<temp_  && temp_<gmax_limit){
                G=MAX_BRIGHT;
           
        }
        if (bmin_limit<temp_ && temp_<bmax_limit){
                B=MAX_BRIGHT;
               
        }

        ESP_LOGI("rmin","rmin %d",rmin_limit);
        ESP_LOGI("rmin","rmax %d",rmax_limit);
        ESP_LOGI("rmin","gmin %d",gmin_limit);
        ESP_LOGI("rmin","gmax %d",gmax_limit);
        ESP_LOGI("rmin","bmin %d",bmin_limit);
        ESP_LOGI("rmin","bmax %d",bmax_limit);

        RGB_set_color(&LED_RGB, R,G,B);
        R=OFF;
        G=OFF;
        B=OFF;
        
        vTaskDelay(10);
    }
    
}



void read_buttom(){
    color_conf color_conf =SET_CONF2;
    while (1)
    {
        if (gpio_get_level(GPIO_NUM_0)==0){ //check if the buttom is pressed
            xQueueReceive(color_to_conf, &color_conf,pdMS_TO_TICKS(100)); //read the colorto configure
            if (color_conf==SET_CONF2)   //check if the previous state was set the color
            {
                color_conf=RED_CONF2;        //prepare to configure the red color
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0)); //send the new color state
            }
            else if (color_conf==RED_CONF2) //check if the previous state was red color
            { 
                color_conf=GREEN_CONF2;  //prepare to configure the green color
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0));//send the new color state
            }
            
            else if(color_conf==GREEN_CONF2){//check if the previous state was green color
                color_conf=BLUE_CONF2; //prepare to configure the blue color
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0));//send the new color state
            }

            else{
                color_conf=SET_CONF2; //if the previous state was blue color, then set the combination RGB
                xQueueSend(color_to_conf, &color_conf, pdMS_TO_TICKS(0));//send the new color state
            }

            while (gpio_get_level(GPIO_NUM_0)==0){
                    vTaskDelay(10); 
            }
        }
     vTaskDelay(10);
    }
}

void conf_color(){

    adc_oneshot_unit_handle_t adc1_handle_=adc_config_(EXAMPLE_ADC_ATTEN,EXAMPLE_ADC2_CHAN4);
    color_conf color_config;
    int adc_value[1][1];
    //initialize the color's intensities
    float RED     =0;
    float GREEN   =0;
    float BLUE    =0;
    while (1)
    {
         // Read ADC value
        adc_oneshot_read(adc1_handle_, EXAMPLE_ADC2_CHAN4, &adc_value[0][0]);
        //read the color from the queue
        xQueueReceive(color_to_conf,&color_config,pdMS_TO_TICKS(100));
        
        /*check what color has to be configured*/
        if (color_config==RED_CONF2)
            {
                RED= (adc_value[0][0]*100)/4095; //transform raw value from adc to a intensity 
                RGB_set_color(&LED_RGB2,RED,0,0); //set the color
                vTaskDelay(10 );//wait
                
            }
        else if (color_config==GREEN_CONF2)
            {
                
                GREEN= (adc_value[0][0]*100)/4095; //transform raw value from adc to a intensity
                RGB_set_color(&LED_RGB2,0,GREEN,0); //set the color
                vTaskDelay(10 );//wait

            }
            
        else if(color_config==BLUE_CONF2)
            {
                
                BLUE= (adc_value[0][0]*100)/4095; //transform raw value from adc to a intensity
                RGB_set_color(&LED_RGB2,0,0,BLUE); //set the color
                vTaskDelay(10);//wait

            }

        else{
            RGB_set_color(&LED_RGB2,RED,GREEN,BLUE); //set the combined color
            vTaskDelay(100 );//wait

        }
        

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
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_IN2;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void app_main(void)
{
    
    UART_init(); //initialize the uart
    RGB_conf( &LED_RGB ); //config the RGB led
    RGB_conf( &LED_RGB2 );
    //configure the buttom as an input
    gpio_config_func();
    //create all the queues
    TEMP_Q= xQueueCreate( 10 , sizeof(uint32_t));
    R_MIN= xQueueCreate( 10 , sizeof(uint32_t));
    R_MAX= xQueueCreate( 10 , sizeof(uint32_t));
    G_MIN= xQueueCreate( 10 , sizeof(uint32_t));
    G_MAX= xQueueCreate( 10 , sizeof(uint32_t));
    B_MIN= xQueueCreate( 10 , sizeof(uint32_t));
    B_MAX= xQueueCreate( 10 , sizeof(uint32_t));
    GET_TEMP_STATUS= xQueueCreate( 10 , sizeof(uint32_t));
    color_to_conf= xQueueCreate( 10 , sizeof(uint32_t));
    //call all the functions as tasks
    xTaskCreatePinnedToCore( &read_ntc , "read ntc" , 2048 , NULL, 5, NULL , 0 );
    xTaskCreatePinnedToCore( &UART_commands , "uart comunication" , 4096 , NULL, 5, NULL , 1);
    xTaskCreatePinnedToCore( &colorconf , "LED RGB color " , 2048 , NULL, 5, NULL , 1);
    xTaskCreatePinnedToCore(&GET_TEMP_FUNCTION , "Get temperatur control " , 2048 , NULL, 5, NULL , 1);
    xTaskCreatePinnedToCore( &read_buttom , "read buttom" , 2048 , NULL, 5, NULL , 0 );
    xTaskCreatePinnedToCore( &conf_color , "change led state" , 2048 , NULL , 5 , NULL , 0 );
}
