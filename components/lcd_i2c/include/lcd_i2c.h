/* 
 * MIT License
 * 
 * Copyright (c) 2022 https://github.com/MuriloAM
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file lcd_i2c.h
 * @defgroup lcd_i2c lcd_i2c
 * @{
 *
 * @brief 
 */
#pragma once

#include "esp_err.h"
#include "i2cbus.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    i2cbus_t bus;
    bool started;
    bool backlight;
} lcd_i2c_t;

/**
 * @brief Create a new lcd on i2c bus.
 * 
 * @param lcd pointer to device configurations.
 * @param port I2C port number lesser than I2C_NUM_MAX.
 * @param addr I2C address to access device on the bus.
 *
 * @return 
 *     - ESP_OK: success
 *     - ESP_FAIL: fail to start
 */
esp_err_t lcd_i2c_init(lcd_i2c_t *lcd, i2c_port_t port, uint8_t addr);

/**
 * @brief Create a new device on bus
 * 
 * @param lcd pointer to device configurations
 *
 * @return 
 *     - ESP_OK: success
 *     - ESP_FAIL: fail to start
 */
esp_err_t lcd_i2c_delete(lcd_i2c_t *lcd);

/**
 * @brief Create a new device on bus
 * 
 * @param lcd pointer to device configurations
 *
 * @return 
 *     - ESP_OK: success
 *     - ESP_FAIL: fail to start
 */
esp_err_t lcd_i2c_clear_display(lcd_i2c_t *lcd);

/**
 * @brief Create a new device on bus
 * 
 * @param dev pointer to device configurations
 *
 * @return 
 *     - ESP_OK: success
 *     - ESP_FAIL: fail to start
 */
esp_err_t lcd_i2c_write(lcd_i2c_t *lcd, const char *data);

/**
 * @brief Create a new device on bus
 * 
 * @param dev pointer to device configurations
 *
 * @return 
 *     - ESP_OK: success
 *     - ESP_FAIL: fail to start
 */
esp_err_t lcd_i2c_set_backlight(lcd_i2c_t *lcd, bool bkl_status);

/**
 * @brief Create a new device on bus
 * 
 * @param dev pointer to device configurations
 *
 * @return 
 *     - ESP_OK: success
 *     - ESP_FAIL: fail to start
 */
esp_err_t lcd_i2c_set_cursor(lcd_i2c_t *lcd, uint8_t col, uint8_t row);


/**@}*/

#ifdef __cplusplus
}
#endif