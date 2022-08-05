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
 * @file i2cbus.c
 * 
 */
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "i2cbus.h"

// LOCAL CONST
#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_INT_FLAG_DISABLE 0                           /*!< I2C master doesn't need buffer */

// LOCAL MACROS
#define I2C_WRITE(addr)     (addr << 1)
#define I2C_READ(addr)      ((addr << 1) | 1)

static const char *TAG = "i2cbus";

typedef struct {
    SemaphoreHandle_t mutex;
    i2c_config_t conf;
    bool installed;
} i2cbus_port_t;

static i2cbus_port_t i2cbus_port[I2C_NUM_MAX];

esp_err_t i2cbus_init(i2c_port_t i2c_port, i2c_mode_t i2c_mode, gpio_num_t i2c_sda, gpio_num_t i2c_scl)
{
    // return if i2c_port already installed.
    if (i2cbus_port[i2c_port].installed)
        return ESP_OK;
    
    // check if i2c_port is valid.
    if (i2c_port >= I2C_NUM_MAX)
        return ESP_ERR_INVALID_ARG;
    
    // clear entire variable.
    memset(&i2cbus_port[i2c_port], 0, sizeof(i2cbus_port_t));
    // create a port mutex semaphore handle.
    i2cbus_port[i2c_port].mutex = xSemaphoreCreateMutex();

    // Check if semaphore was created.
    if (i2cbus_port[i2c_port].mutex != NULL) {
        // The semaphore was created successfully.
        // The semaphore can now be used.
        i2cbus_port[i2c_port].installed = true;

    // The semaphore wasn't created return ESP_FAIL.
    } else {
        i2cbus_port[i2c_port].installed = false;
        return ESP_FAIL;
    } 

    // installing i2c driver.
    i2c_config_t conf = {
        .mode = i2c_mode,
        .sda_io_num = i2c_sda,
        .scl_io_num = i2c_scl,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ,
    };

    esp_err_t res = ESP_FAIL;
    res = i2c_param_config(i2c_port, &conf);

    res = i2c_driver_install(i2c_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 
                              I2C_MASTER_INT_FLAG_DISABLE);
    return res;
}

esp_err_t i2cbus_create(i2cbus_t *dev, i2c_port_t i2c_port, uint8_t addr)
{
    if (!dev) 
        return ESP_ERR_INVALID_ARG;
    
    // Create a new semaphore handler for this device.
    dev->mutex = xSemaphoreCreateMutex();

    if (!dev->mutex) 
        return ESP_FAIL;
    
    dev->port = i2c_port;
    dev->addr = addr;
    dev->time_out = I2C_TIMEOUT;

    ESP_LOGI(TAG, "new device has been created");
    return ESP_OK;
}

esp_err_t i2cbus_delete(i2cbus_t *dev)
{
    if (dev->mutex != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(dev->mutex, pdMS_TO_TICKS(dev->time_out)) == pdTRUE) {
            // Delete semaphore.
            vSemaphoreDelete(dev->mutex);
            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(dev->mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            return ESP_ERR_TIMEOUT;
        }
    }

    ESP_LOGI(TAG, "device has been deleted");
    return ESP_OK;
}

esp_err_t i2cbus_write_reg(i2cbus_t *dev, uint8_t *reg, size_t reg_size, uint8_t *data, size_t data_size)
{
    esp_err_t res = ESP_ERR_INVALID_ARG;

    if (dev != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(i2cbus_port[dev->port].mutex, pdMS_TO_TICKS(dev->time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.
            
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            // add a start condition in buffer.
            i2c_master_start(cmd);
            // set address device in buffer.
            i2c_master_write_byte(cmd, I2C_WRITE(dev->addr), true);

            // check if a register must be selected before data in buffer.
            if (reg && reg_size)
                i2c_master_write(cmd, (void *)reg, reg_size, true);
            
            // write data do buffer.
            i2c_master_write(cmd, (void *)data, data_size, true);
            // add a stop condition in buffer.
            i2c_master_stop(cmd);
            res = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(dev->time_out));
            if (res != ESP_OK)
                ESP_LOGE(TAG, "Device not found [0x%02x at %d]: %d (%s)", dev->addr, dev->port, res, esp_err_to_name(res));
            
            i2c_cmd_link_delete(cmd);

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(i2cbus_port[dev->port].mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            res = ESP_ERR_TIMEOUT;
        }
    }
    return res;
}

esp_err_t i2cbus_read_reg(i2cbus_t *dev, uint8_t *reg, size_t reg_size, uint8_t *data, size_t data_size)
{
    esp_err_t res = ESP_ERR_INVALID_ARG;

    if (dev != NULL) {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.
        if (xSemaphoreTake(i2cbus_port[dev->port].mutex, pdMS_TO_TICKS(dev->time_out)) == pdTRUE) {
            // We were able to obtain the semaphore and can now access the
            // shared resource.

            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            // Select a register to read if needs.
            if (reg && reg_size) {
                i2c_master_start(cmd);
                i2c_master_write_byte(cmd, I2C_WRITE(dev->addr), true);
                i2c_master_write(cmd, (void *)reg, reg_size, true);
            }
            // add a re-start condition in buffer.
            i2c_master_start(cmd);
            // set address device in buffer.
            i2c_master_write_byte(cmd, I2C_READ(dev->addr), true);
            // read data do buffer.
            i2c_master_read(cmd, data, data_size, I2C_MASTER_LAST_NACK);
            // add a stop condition in buffer.
            i2c_master_stop(cmd);
            res = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(dev->time_out));
            if (res != ESP_OK) 
                ESP_LOGE(TAG, "Device not found [0x%02x at %d]: %d (%s)", dev->addr, dev->port, res, esp_err_to_name(res));
            
            i2c_cmd_link_delete(cmd);
            ESP_LOGI(TAG, "data=%x", *data);

            // We have finished accessing the shared resource.  Release the
            // semaphore.
            xSemaphoreGive(i2cbus_port[dev->port].mutex);
        }
        else {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
            res = ESP_ERR_TIMEOUT;
        }
    }
    return res;
}

esp_err_t i2cbus_write(i2cbus_t *dev, uint8_t *data, size_t data_size)
{
    return i2cbus_write_reg(dev, 0, 0, data, data_size);
}

esp_err_t i2cbus_read(i2cbus_t *dev, uint8_t *data, size_t data_size)
{
    return i2cbus_read_reg(dev, 0, 0, data, data_size);
}