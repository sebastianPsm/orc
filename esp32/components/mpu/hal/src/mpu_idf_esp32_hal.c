#include <mpu_idf_esp32_hal.h>
#include <driver/eMPL/inv_mpu_dmp_motion_driver.h>
//#define CONFIG_I2CBUS_LOG_READWRITES
//#define CONFIG_I2CBUS_LOG_ERRORS

#define tag "MPL"

int esp32_delay_ms(unsigned long num_ms) {
    vTaskDelay(num_ms / portTICK_PERIOD_MS);
    return 0;
}
int esp32_get_clock_ms(unsigned long *count){
    *count = esp_timer_get_time()/1000;
    return 0;
}

typedef struct {
    i2c_port_t port;
    int sda_io_num;
    bool sda_pullup_en;
    int scl_io_num;
    bool scl_pullup_en;
    uint32_t clk_speed;
    TickType_t timeout_ticks;
} tEsp32MotionDriverI2C;

tEsp32MotionDriverI2C * handle = NULL;
TaskHandle_t update_task;

static xQueueHandle gpio_evt_queue = NULL;

#define I2C_MASTER_ACK_EN   true    /*!< Enable ack check for master */
#define I2C_MASTER_ACK_DIS  false   /*!< Disable ack check for master */

esp_err_t esp32_i2c_init(i2c_port_t port, int sda_io_num, bool sda_pullup_en, int scl_io_num, bool scl_pullup_en, uint32_t clk_speed, uint32_t timeout_ms) {
    if(handle != NULL) {
        ESP_LOGE(tag, "Already initialized");
        return ESP_FAIL;
    }
    handle = (tEsp32MotionDriverI2C *) malloc(sizeof(tEsp32MotionDriverI2C));
    handle->port = port;
    handle->sda_io_num = sda_io_num;
    handle->sda_pullup_en = sda_pullup_en;
    handle->scl_io_num = scl_io_num;
    handle ->scl_pullup_en = scl_pullup_en;
    handle->clk_speed = clk_speed;
    handle->timeout_ticks = (TickType_t) pdMS_TO_TICKS(timeout_ms);

    // init ESP32 i2c
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_io_num;
    conf.sda_pullup_en = sda_pullup_en;
    conf.scl_io_num = scl_io_num;
    conf.scl_pullup_en = scl_pullup_en;
    conf.master.clk_speed = clk_speed;
    esp_err_t err = i2c_param_config(port, &conf);
    if (!err) err = i2c_driver_install(port, conf.mode, 0, 0, 0);

    return err;
}
esp_err_t esp32_i2c_term() {
    if(handle == NULL) {
        ESP_LOGE(tag, "Not  initialized");
        return ESP_FAIL;
    }

    esp_err_t res = i2c_driver_delete(handle->port);
    free(handle); handle = NULL;

    return res;
}

int esp32_i2c_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data) {
    if(handle == NULL) {
        ESP_LOGE(tag, "Not  initialized");
        return (int) ESP_FAIL;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, reg_addr, I2C_MASTER_ACK_EN);
    i2c_master_write(cmd, (uint8_t*) data, length, I2C_MASTER_ACK_EN);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(handle->port, cmd, handle->timeout_ticks);
    i2c_cmd_link_delete(cmd);
#if defined CONFIG_I2CBUS_LOG_READWRITES
    if (!err) {
        char str[length*5+1];
        for (size_t i = 0; i < length; i++)
            sprintf(str+i*5, "0x%s%X ", (data[i] < 0x10 ? "0" : ""), data[i]);
            printf("[port:%d, slave:0x%X] Write %d bytes to register 0x%X, data: %s\n", handle->port, slave_addr, length, reg_addr, str);
    }
#endif
#if defined CONFIG_I2CBUS_LOG_ERRORS
#if defined CONFIG_I2CBUS_LOG_READWRITES
    else {
#else
    if (err) {
#endif
        printf("ERROR: [port:%d, slave:0x%X] Failed to write %d bytes to__ register 0x%X, error: 0x%X\n", handle->port, slave_addr, length, reg_addr, err);
    }
#endif
    return err;
    return 0;
                     }
int esp32_i2c_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data) {
    if(handle == NULL) {
        ESP_LOGE(tag, "Not  initialized");
        return (int) ESP_FAIL;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, reg_addr, I2C_MASTER_ACK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, I2C_MASTER_ACK_EN);
    i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(handle->port, cmd, handle->timeout_ticks);
    i2c_cmd_link_delete(cmd);
#if defined CONFIG_I2CBUS_LOG_READWRITES
    if (!err) {
        char str[length*5+1];
        for (size_t i = 0; i < length; i++)
        sprintf(str+i*5, "0x%s%X ", (data[i] < 0x10 ? "0" : ""), data[i]);
        printf("[port:%d, slave:0x%X] Read_ %d bytes from register 0x%X, data: %s\n", handle->port, slave_addr, length, reg_addr, str);
    }
#endif
#if defined CONFIG_I2CBUS_LOG_ERRORS
#if defined CONFIG_I2CBUS_LOG_READWRITES
    else {
#else
    if (err) {
#endif
        printf("ERROR: [port:%d, slave:0x%X] Failed to read %d bytes from register 0x%X, error: 0x%X\n", handle->port, slave_addr, length, reg_addr, err);
    }
#endif
    return err;
    return 0;
}
IRAM_ATTR void mpu_isr_handler(void * data) {
    uint32_t in = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(gpio_evt_queue, &in, &xHigherPriorityTaskWoken);
}
void mpu_read_task(void * data) {
    struct int_param_s * int_param = (struct int_param_s *) data;

        uint32_t out = 0;
        for(;;) {
            xQueueReceive(gpio_evt_queue, &out, portMAX_DELAY);
            
            if(int_param->cb != NULL)
                int_param->cb(int_param->arg);
        }    
}
int reg_int_cb(struct int_param_s *int_param) {
    /*
     * Setup interrupt
     */
    gpio_config_t kGPIOConfig;
    kGPIOConfig.intr_type = GPIO_INTR_POSEDGE;
    kGPIOConfig.pin_bit_mask = (uint64_t) 0x1 << (gpio_num_t) int_param->interrupt_pin;
    kGPIOConfig.mode = GPIO_MODE_INPUT;
    kGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    kGPIOConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&kGPIOConfig);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(mpu_read_task, "mpu_read_task", 4 * 1024, (void *) int_param, 5, &update_task);

    gpio_install_isr_service(0); // ESP_INTR_FLAG_DEFAULT
    gpio_isr_handler_add((gpio_num_t) int_param->interrupt_pin, mpu_isr_handler, (void *) int_param);   

    return 0;
}
inline void __no_operation() {
    asm("nop");
}