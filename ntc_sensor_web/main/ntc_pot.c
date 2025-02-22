#include "ntc_pot.h"

static esp_adc_cal_characteristics_t *adc_chars;

// void read_ntc(){
//     //create variables to store the raw and voltage values
//     int adc_value[1][1];
//     int voltage_value[1][1];
//     int temperature;
//     adc_oneshot_unit_handle_t adc1_handle=adc__oneshot_config_(ADC1_ATTEN,ADC1_CHAN_6); //conf adc1 chan 6
//     adc_cali_handle_t adc1_cali_chan0_handle = NULL;
//     bool do_calibration1_chan0 = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN_6, ADC1_ATTEN, &adc1_cali_chan0_handle);//calibration
//     for(;;){
//         adc_oneshot_read(adc1_handle, ADC1_CHAN_6, &adc_value[0][0]);//read raw
//         if(do_calibration1_chan0){
//         adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_value[0][0], &voltage_value[0][0]);}//REad voltage
//         temperature = (-0.033324*voltage_value[0][0])+78.22 ; //convert voltage to temperaure value in °c
//         xQueueSend( TEMP_Q , &temperature , pdMS_TO_TICKS(0)); //send the temperature
        
//         vTaskDelay(100);
//     }

// }

void GET_TEMP_FUNCTION(){
    int status=1;
    char message_to_send[RX_BUF_SIZE+1];
    int temp;
    while (1)
    {
       xQueueReceive(TEMP_Q_UART , &temp , 100/ portTICK_PERIOD_MS);
       xQueueReceive(GET_TEMP_STATUS,&status,100/ portTICK_PERIOD_MS);
       
       if (status!=0){
            if(status==1){ 
                sprintf(message_to_send,"la temperatura es %d \n ",temp);
                sendData(message_to_send);
                vTaskDelay(pdMS_TO_TICKS(2000));

            }
        }
        vTaskDelay(10);
         
    }
}

// void read_pot(){
//     adc_oneshot_unit_handle_t adc1_handle_=adc__oneshot_config_(ADC1_ATTEN, ADC1_CHAN_7);
//     int adc_value[1][1];
//     int voltage_value[1][1];
    
//     adc_cali_handle_t adc1_cali_chan4_handle = NULL;
//     bool do_calibration1_chan0 = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN_7, ADC1_ATTEN, &adc1_cali_chan4_handle);//calibration
    
   
    

    
//     for(;;){
//         adc_oneshot_read(adc1_handle_, ADC1_CHAN_7, &adc_value[0][0]);//read raw
       
//         if(do_calibration1_chan0){
//             adc_cali_raw_to_voltage(adc1_cali_chan4_handle, adc_value[0][0], &voltage_value[0][0]);}//REad voltage
//         int raw=adc_value[0][0];
//         xQueueSend( VOL_Q , &raw , pdMS_TO_TICKS(0)); //send the temperature
        
//         vTaskDelay(10);
//     }

// }


void read_pot_ntc(){
    int adc_value_temp;
    int voltage_value_temp;
    int adc_value_pot;
    int voltage_value_pot;
    int temperature;
    float voltage;
    adc_chars=adc_init_2_channels(ADC1_CHAN_6,ADC1_CHAN_7,ADC1_ATTEN);

    
    
    for(;;){
        
        adc_value_temp= adc1_get_raw(ADC1_CHAN_6);
        adc_value_pot = adc1_get_raw(ADC1_CHAN_7);
        
        voltage_value_temp=esp_adc_cal_raw_to_voltage(adc_value_temp, adc_chars);
        voltage_value_pot =esp_adc_cal_raw_to_voltage(adc_value_pot, adc_chars);
        
        temperature = (-0.033324*voltage_value_temp)+78.22 ; //convert voltage to temperaure value in °c
        voltage = voltage_value_pot;

        xQueueSend( VOL_Q , &voltage , pdMS_TO_TICKS(0)); //send the temperature
        xQueueSend( TEMP_Q , &temperature , pdMS_TO_TICKS(0)); //send the temperature
        xQueueSend( TEMP_Q_UART , &temperature , pdMS_TO_TICKS(0)); //send the temperature
        
        vTaskDelay(100);
    }
}
