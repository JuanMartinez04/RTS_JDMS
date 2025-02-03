#ifndef ADC_LIB_H
#define ADC_LIB_H
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"



const static char *TAG = "OUTPUT";
/**
 * @brief Configures and initializes an ADC unit with the specified attenuation and channel.
 * 
 * @param atten   ADC attenuation level (affects input voltage range).
 * @param channel ADC channel to be configured.
 * @return adc_oneshot_unit_handle_t Handle to the initialized ADC unit.
 */

adc_oneshot_unit_handle_t adc_config( adc_atten_t atten,adc_channel_t channel);
adc_oneshot_unit_handle_t adc_config_( adc_atten_t atten_,adc_channel_t channel_);
/**
 * @brief Initializes ADC calibration using the Line Fitting scheme.
 * 
 * @param unit      ADC unit (ADC_UNIT_1 or ADC_UNIT_2).
 * @param channel   ADC channel to be calibrated.
 * @param atten     ADC attenuation level.
 * @param out_handle Pointer to store the calibration handle.
 * @return true if calibration was successful, false otherwise.
 */
bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, 
                                adc_atten_t atten, adc_cali_handle_t *out_handle);



#endif // ADC_DRIVER_H