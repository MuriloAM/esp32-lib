#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "i2cbus.h"
#include "lcd_i2c.h"

static const char *TAG = "main";

void vTaskMain(void *pvParameters)
{
    lcd_i2c_t lcd;
    memset(&lcd, 0, sizeof(lcd_i2c_t));

    lcd_i2c_init(&lcd, I2C_NUM_0, 0x3F);
    
    ESP_LOGI(TAG, "is running");
    // inicializacoes.
    while (true) {
        /* main loop */
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    i2cbus_init(I2C_NUM_0, I2C_MODE_MASTER, I2C_MASTER_SDA, I2C_MASTER_SCL);

    xTaskCreatePinnedToCore(vTaskMain, "vTaskMain", 1024*2, NULL, 5, NULL, APP_CPU_NUM);
}