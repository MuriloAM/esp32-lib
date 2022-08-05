#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "i2cbus.h"
#include "lcd_i2c.h"

// static const char *TAG = "main";

void vTaskMain(void *pvParameters)
{
    i2cbus_t pcf8574;
    uint8_t data = 0x00;
    i2cbus_create(&pcf8574, I2C_NUM_0, 0x30);
    i2cbus_write(&pcf8574, &data, sizeof(data));
    while (true) {
        /* main loop */
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
}

void app_main(void)
{
    // start i2c bus.
    i2cbus_init(I2C_NUM_0, I2C_MODE_MASTER, I2C_MASTER_SDA, I2C_MASTER_SCL);

    xTaskCreatePinnedToCore(vTaskMain, "vTaskMain", 1024*2, NULL, 5, NULL, APP_CPU_NUM);
}