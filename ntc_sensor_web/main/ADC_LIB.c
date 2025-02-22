#include "ADC_LIB.h"



// adc_oneshot_unit_handle_t adc_config( adc_atten_t atten,adc_channel_t channel ){
//     adc_oneshot_unit_handle_t handle;
//     adc_oneshot_unit_init_cfg_t init_config1 = {
//         .unit_id = ADC_UNIT_1,
//     };
//     // Initialize ADC unit
//     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));

//     //-------------ADC Config---------------//
//     adc_oneshot_chan_cfg_t config = {
//         .bitwidth = ADC_BITWIDTH_DEFAULT,
//         .atten = atten,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, channel, &config));
//     return handle;
// }



// adc_oneshot_unit_handle_t adc__oneshot_config_( adc_atten_t atten , adc_channel_t channel) {
//     // Inicializar ADC1 una sola vez
//     static bool initialized = false;
//     adc_oneshot_unit_handle_t handle;
//     if (!initialized) {
//         adc_oneshot_unit_init_cfg_t init_config = {
//             .unit_id = ADC_UNIT_1
//         };
//         ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &handle));
//         initialized = true;
//     }

//     // Configurar los canales especificados
//     adc_oneshot_chan_cfg_t config = {
//         .bitwidth = ADC_BITWIDTH_DEFAULT,
//         .atten = atten,
//     };

    
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, channel, &config));
    

//     return handle;
// }

// bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
// {
//     static bool calibrated_channels[8] = {false}; // Estado de calibración por canal
//     adc_cali_handle_t handle = NULL;
//     esp_err_t ret = ESP_FAIL;

//     if (!calibrated_channels[channel]) { // Solo calibrar si el canal no ha sido calibrado aún
//         ESP_LOGI(TAG_, "Calibrando canal %d con esquema Line Fitting", channel);
//         adc_cali_line_fitting_config_t cali_config = {
//             .unit_id = unit,
//             .atten = atten,
//             .bitwidth = ADC_BITWIDTH_DEFAULT,
//         };
//         ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
//         if (ret == ESP_OK) {
//             calibrated_channels[channel] = true; // Marcar canal como calibrado
//         }
//     }

//     *out_handle = handle;
//     if (ret == ESP_OK) {
//         ESP_LOGI(TAG_, "Calibración del canal %d exitosa", channel);
//     } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated_channels[channel]) {
//         ESP_LOGW(TAG_, "eFuse no quemado, omitiendo calibración en canal %d", channel);
//     } else {
//         ESP_LOGE(TAG_, "Error en calibración del canal %d: Argumento inválido o falta de memoria", channel);
//     }

//     return calibrated_channels[channel]; // Retorna true solo si el canal se calibró correctamente
// }

esp_adc_cal_characteristics_t *adc_init_2_channels(adc_channel_t channel1, adc_channel_t channel2,adc_atten_t atten){
    // Configurar el ADC1
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel1, atten);
    adc1_config_channel_atten(channel2, atten);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  
    esp_adc_cal_characterize(ADC_UNIT_1, atten, ADC_WIDTH_BIT_12, 3.3, adc_chars);
    return adc_chars;
}