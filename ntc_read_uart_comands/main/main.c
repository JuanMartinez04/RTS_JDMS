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

/* Define FreeRTOS queues to store temperature and RGB thresholds */
QueueHandle_t TEMP_Q;

QueueHandle_t R_MIN;
QueueHandle_t R_MAX;

QueueHandle_t G_MIN;
QueueHandle_t G_MAX;

QueueHandle_t B_MIN;
QueueHandle_t B_MAX;




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
 * @brief NTC read function
 * 
 * 
 */

void read_ntc(){
    //create variables to store the raw and voltage values
    int adc_value[1][1];
    int voltage_value[1][1];
    int temperature=0;
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

/**
 * @brief Splits an input string into three words
 * 
 * @param input Input string
 * @param words Array to store the three extracted words
 */
void str_to_chars(const char *input, char words[3][1025]) {
    char copy[1025]; //make a copy of the original string
    strncpy(copy, input, 1025 - 1);
    copy[1025 - 1] = '\0';

    const char delimitador[] = " ";
    char *token = strtok(copy, delimitador);

    for (int i = 0; i < 3; i++) {
        if (token != NULL) {
            strncpy(words[i], token, 1025 - 1);
            words[i][1025 - 1] = '\0';
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
    int temp=0;
    char message[1025];
    
     while (1) {

        xQueueReceive(TEMP_Q,&temp, 100/ portTICK_PERIOD_MS);

        char *receive_c = rx_data(); //read uart 

        if (receive_c!= NULL){ //if string is not null
            char (*words)[1025] = malloc(3 * 1025); 
            str_to_chars(receive_c,words);
            
            
            if (strcmp(words[0],"GET_TEMP")==0) {
                
                sprintf(message, "the temperature is: %d \n",temp);
                sendData(message);
                free(words);
                
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

    int rmax_limit=0;
    int rmin_limit=0;

    int gmin_limit=0;
    int gmax_limit=0;

    int bmin_limit=0;
    int bmax_limit=0;

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
                R=99;
             
        }
        if (gmin_limit<temp_  && temp_<gmax_limit){
                G=99;
           
        }
        if (bmin_limit<temp_ && temp_<bmax_limit){
                B=99;
               
        }

        ESP_LOGI("rmin","rmin %d",rmin_limit);
        ESP_LOGI("rmin","rmax %d",rmax_limit);
        ESP_LOGI("rmin","gmin %d",gmin_limit);
        ESP_LOGI("rmin","gmax %d",gmax_limit);
        ESP_LOGI("rmin","bmin %d",bmin_limit);
        ESP_LOGI("rmin","bmax %d",bmax_limit);

        RGB_set_color(&LED_RGB, R,G,B);
        R=0;
        G=0;
        B=0;
        
        vTaskDelay(10);
    }
    
}


    

void app_main(void)
{
    
    UART_init(); //initialize the uart
    RGB_conf( &LED_RGB ); //config the RGB led
    //create all the queues
    TEMP_Q= xQueueCreate( 10 , sizeof(uint32_t));
    R_MIN= xQueueCreate( 10 , sizeof(uint32_t));
    R_MAX= xQueueCreate( 10 , sizeof(uint32_t));
    G_MIN= xQueueCreate( 10 , sizeof(uint32_t));
    G_MAX= xQueueCreate( 10 , sizeof(uint32_t));
    B_MIN= xQueueCreate( 10 , sizeof(uint32_t));
    B_MAX= xQueueCreate( 10 , sizeof(uint32_t));
    //call all the functions as tasks
    xTaskCreatePinnedToCore( &read_ntc , "read ntc" , 2048 , NULL, 5, NULL , 0 );
    xTaskCreatePinnedToCore( &UART_commands , "uart comunication" , 4096 , NULL, 5, NULL , 1);
    xTaskCreatePinnedToCore( &colorconf , "LED RGB color " , 2048 , NULL, 5, NULL , 1);
    

}
