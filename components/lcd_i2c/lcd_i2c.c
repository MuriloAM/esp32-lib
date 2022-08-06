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
 * @file lcd_i2c.c
 * 
 */
#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp32/rom/ets_sys.h"
#include "lcd_i2c.h"
#include "lcd_i2c_const.h"

static const char *TAG = "lcd_i2c";

esp_err_t _lcd_i2c_write(lcd_i2c_t *lcd, uint8_t data, lcd_i2c_reg_t lcd_reg)
{
    if (lcd != NULL) {
        for (int wr_seq = 0; wr_seq < 2; wr_seq ++) {
            uint8_t _data;
            // select witch nibble of data will be writing at time.
            if (wr_seq) {
                if (!lcd->started) {
                    if ((data == LCD_CONFIG_4BIT_RST) && 
                        (lcd_reg == LCD_I2C_INSTRUCTION))
                        lcd->started = true;
                    return ESP_OK;
                }
                // send LSB.
                _data = SHFT_LEFT(LOW_NIBBLE(data), MOVE_NIBBLE);
            } else {
                // send MSB.
                _data = HIGH_NIBBLE(data);
            }

            // switch register to write instruction or data.
            if (lcd_reg)
                SET_BIT(_data, LCD_BIT_RS);
            
            // check backlight status and add it's bit to i2c byte.
            if (lcd->backlight) 
                SET_BIT(_data, LCD_BIT_BKL);
            
            // transmit the data to lcd via i2c bus, with enable sign.
            for (int en_bit = 0; en_bit < 3; en_bit++) {
                // switch enable bit and populate on data byte.
                if (en_bit % 2)
                    SET_BIT(_data, LCD_BIT_EN);
                else
                    CLR_BIT(_data, LCD_BIT_EN);

                if (i2cbus_write(&lcd->bus, &_data, sizeof(_data)) != ESP_OK)
                    return ESP_FAIL;
                
                ets_delay_us(DELAY_EN);
            }
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    if (!lcd_reg && (data < 4)) {
        ets_delay_us(DELAY_CLR);
    }
    
    return ESP_OK;
}

esp_err_t lcd_i2c_init(lcd_i2c_t *lcd, i2c_port_t port, uint8_t addr, lcd_type_t lcd_type)
{
    // create lcd mutex semaphore, return fail if can't create.
    if (i2cbus_create(&lcd->bus, port, addr) != ESP_OK)
        return ESP_FAIL;
    
    lcd->backlight = true;
    lcd->type = lcd_type;

    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            ets_delay_us(DELAY_PWON);
            // starting reset device procedment.
            _lcd_i2c_write(lcd, LCD_CONFIG_8BIT_RST, LCD_I2C_INSTRUCTION);
            ets_delay_us(DELAY_RST);
            _lcd_i2c_write(lcd, LCD_CONFIG_8BIT_RST, LCD_I2C_INSTRUCTION);
            ets_delay_us(DELAY_RST);
            _lcd_i2c_write(lcd, LCD_CONFIG_8BIT_RST, LCD_I2C_INSTRUCTION);
            _lcd_i2c_write(lcd, LCD_CONFIG_4BIT_RST, LCD_I2C_INSTRUCTION);

            // set-up display comunication bits, number of lines and 
            // character size.
            _lcd_i2c_write(lcd, LCD_CONFIG_4BIT_2LINE_5X7, LCD_I2C_INSTRUCTION);
            
            // finish reset procedment.
            _lcd_i2c_write(lcd, LCD_DISPLAY_OFF, LCD_I2C_INSTRUCTION);
            _lcd_i2c_write(lcd, LCD_CLR_DISPLAY, LCD_I2C_INSTRUCTION);
            _lcd_i2c_write(lcd, LCD_WRITE_TO_RIGHT, LCD_I2C_INSTRUCTION);

            // turn on display and it is restarted done.
            _lcd_i2c_write(lcd, LCD_DISPLAY_ON, LCD_I2C_INSTRUCTION);

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "init");
    return ESP_OK;
}

esp_err_t lcd_i2c_delete(lcd_i2c_t *lcd)
{
    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            vSemaphoreDelete(lcd->bus.mutex);

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t lcd_i2c_clear_display(lcd_i2c_t *lcd)
{
    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            _lcd_i2c_write(lcd, LCD_CLR_DISPLAY, LCD_I2C_INSTRUCTION);

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t lcd_i2c_write(lcd_i2c_t *lcd, const char *data)
{
    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            while (*data) {
                _lcd_i2c_write(lcd, *data, LCD_I2C_DATA);
                data++;
            }
            
            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t lcd_i2c_set_backlight(lcd_i2c_t *lcd, bool bkl_status)
{
    lcd->backlight = bkl_status;

    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            uint8_t _data = 0x00;
            // check backlight status and add it's bit to i2c byte.
            if (lcd->backlight) 
                SET_BIT(_data, LCD_BIT_BKL);
            // send backlight update.
            if (i2cbus_write(&lcd->bus, &_data, sizeof(_data)) != ESP_OK)
                return ESP_FAIL;
            
            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t lcd_i2c_set_cursor(lcd_i2c_t *lcd, uint8_t col, uint8_t row)
{
    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            // col can't reach out last display's column address, it makes 
            // cursor jump to another line.
            if (lcd->type) {
                if (col > LCD_2004_MAX_COL)
                    col = LCD_2004_MAX_COL;
            } else {
                if (col > LCD_1602_MAX_COL)
                    col = LCD_1602_MAX_COL;
            }

            // set DDRAM address.
            _lcd_i2c_write(lcd, (LCD_DDRAM_ADDR + col + lcd_line[row]), LCD_I2C_INSTRUCTION);
            
            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t lcd_i2c_set_cursor_style(lcd_i2c_t *lcd, lcd_i2c_cursor_style_t style)
{
    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.
            switch(style) {
                case LCD_CURSOR_INVISIBLE:
                    _lcd_i2c_write(lcd, LCD_CURSOR_OFF, LCD_I2C_INSTRUCTION);
                break;
                case LCD_CURSOR_UNDERSCORE:
                    _lcd_i2c_write(lcd, LCD_CURSOR_UND, LCD_I2C_INSTRUCTION);
                break;
                case LCD_CURSOR_UNDERSCORE_BLINK:
                    _lcd_i2c_write(lcd, LCD_CURSOR_UND_BLK, LCD_I2C_INSTRUCTION);
                break;
                case LCD_CURSOR_BLINK:
                    _lcd_i2c_write(lcd, LCD_CURSOR_BLK, LCD_I2C_INSTRUCTION);
                break;
                default:
                break;
            }

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t lcd_i2c_shift_display(lcd_i2c_t *lcd, lcd_i2c_shift_display_t direction)
{
    if (lcd != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(lcd->bus.mutex, pdMS_TO_TICKS(lcd->bus.time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.
            switch(direction) {
                case LCD_SHIFT_LEFT:
                    _lcd_i2c_write(lcd, LCD_DISPLAY_MOVE_LEFT, LCD_I2C_INSTRUCTION);
                break;
                case LCD_SHIFT_RIGHT:
                    _lcd_i2c_write(lcd, LCD_DISPLAY_MOVE_RIGHT, LCD_I2C_INSTRUCTION);
                break;
                default:
                break;
            }

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(lcd->bus.mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}