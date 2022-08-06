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
    // creating two lcd objects.
    lcd_i2c_t lcd1602, lcd2004;
    memset(&lcd1602, 0, sizeof(lcd_i2c_t));
    memset(&lcd2004, 0, sizeof(lcd_i2c_t));
    // starting.
    lcd_i2c_init(&lcd1602, I2C_NUM_0, 0x27, LCD_1602);
    lcd_i2c_init(&lcd2004, I2C_NUM_0, 0x3F, LCD_2004);
    // set cursor style
    lcd_i2c_set_cursor_style(&lcd1602, LCD_CURSOR_UNDERSCORE_BLINK);
    lcd_i2c_set_cursor_style(&lcd2004, LCD_CURSOR_BLINK);
    // writing in lcd2004
    lcd_i2c_set_cursor(&lcd2004, 0, 0);
    lcd_i2c_write(&lcd2004, "LINE0");

    lcd_i2c_set_cursor(&lcd2004, 0, 1);
    lcd_i2c_write(&lcd2004, "LINE1");

    lcd_i2c_set_cursor(&lcd2004, 0, 2);
    lcd_i2c_write(&lcd2004, "LINE2");

    lcd_i2c_set_cursor(&lcd2004, 0, 3);
    lcd_i2c_write(&lcd2004, "LINE3");

    ESP_LOGI(TAG, "is running");

    unsigned int value =0;
    // inicializacoes.
    while (true) {
        /* main loop */
        char str_value[10];
        sprintf(str_value, "%.8d", value);
        lcd_i2c_set_cursor(&lcd1602, 0, 0);
        lcd_i2c_write(&lcd1602, str_value);
        lcd_i2c_shift_display(&lcd2004, LCD_SHIFT_LEFT);
        value ++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    i2cbus_init(I2C_NUM_0, I2C_MODE_MASTER, I2C_MASTER_SDA, I2C_MASTER_SCL);

    xTaskCreatePinnedToCore(vTaskMain, "vTaskMain", 1024*2, NULL, 5, NULL, APP_CPU_NUM);
}