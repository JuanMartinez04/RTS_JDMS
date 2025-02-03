#include "ADC_LIB.h"



adc_oneshot_unit_handle_t adc_config( adc_atten_t atten,adc_channel_t channel ){
    adc_oneshot_unit_handle_t handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    // Initialize ADC unit
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));

    //-------------ADC Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = atten,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, channel, &config));
    return handle;
}


adc_oneshot_unit_handle_t adc_config_( adc_atten_t atten_,adc_channel_t channel_ ){
    adc_oneshot_unit_handle_t handle_;
    adc_oneshot_unit_init_cfg_t init_config1_ = {
        .unit_id = ADC_UNIT_2,
    };
    // Initialize ADC unit
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1_, &handle_));

    //-------------ADC Config---------------//
    adc_oneshot_chan_cfg_t config_ = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = atten_,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle_, channel_, &config_));
    return handle_;
}



bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;



    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }


    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}
