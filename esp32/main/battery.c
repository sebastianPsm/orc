#include "battery.h"

#define tag "battery"

static const adc_unit_t unit = ADC_UNIT_1;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static esp_adc_cal_characteristics_t *adc_chars;

static void check_efuse(void) {
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        ESP_LOGI(tag, "eFuse Two Point: Supported");
    } else {
        ESP_LOGI(tag, "eFuse Two Point: NOT supported");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        ESP_LOGI(tag, "eFuse Vref: Supported");
    } else {
        ESP_LOGI(tag, "eFuse Vref: NOT supported");
    }

}
static void print_char_val_type(esp_adc_cal_value_t val_type) {
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI(tag, "Characterized using Two Point Value");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(tag, "Characterized using eFuse Vref");
    } else {
        ESP_LOGI(tag, "Characterized using Default Vref");
    }
}
void battery_init() {
    check_efuse();

    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, ESP_ADC_CAL_VAL_DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}
float getBatterySoc() {
    //Continuously sample ADC1
    uint32_t adc_reading = 0;

    //Multisampling
    for (int i = 0; i < 10; i++)
        adc_reading += adc1_get_raw(channel);
    adc_reading /= 10;

    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    return (float) voltage / 1e3 * 2.7857;
}