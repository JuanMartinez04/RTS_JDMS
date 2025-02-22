#ifndef NTC_POT_H
#define NTC_POT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"


#include "ADC_LIB.h"
#include "UART_LIB.h"



/*Define the adc configuration parameters*/
#define ADC1_CHAN_6                  ADC_CHANNEL_6
#define ADC1_ATTEN                 ADC_ATTEN_DB_11


/*Define the adc configuration parameters*/
#define ADC1_CHAN_7                  ADC_CHANNEL_7



extern QueueHandle_t TEMP_Q;
extern QueueHandle_t TEMP_Q_UART;
extern QueueHandle_t GET_TEMP_STATUS;
extern QueueHandle_t VOL_Q;


//void read_ntc();
void GET_TEMP_FUNCTION();
//void read_pot();
void read_pot_ntc();

#endif