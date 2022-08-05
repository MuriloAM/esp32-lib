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
 * @file i2cbus.h
 * @defgroup i2cbus i2cbus
 * @{
 *
 * @brief .
 */
#pragma once

#include "esp_err.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_MASTER_SCL      CONFIG_I2C_MASTER_SCL   /*!< I2C pin clock line */
#define I2C_MASTER_SDA      CONFIG_I2C_MASTER_SDA   /*!< I2C pin data line */
#define I2C_MASTER_FREQ     CONFIG_I2C_MASTER_FREQ  /*!< I2C master clock frequency */
#define I2C_TIMEOUT         CONFIG_I2C_TIMEOUT      /*!< I2C timeout */

typedef struct 
{
    i2c_port_t port;            /*!< I2C port to access */
    SemaphoreHandle_t mutex;    /*!< Device mutex semaphore */
    uint8_t addr;               /*!< Device address */
    uint32_t time_out;          /*!< I2C comunication timeout */
} i2cbus_t;


/**
 * @brief Initiate i2cbus and start thread safe controll.
 * 
 * @param i2c_port I2C port number lesser than I2C_NUM_MAX.
 * @param i2c_mode I2C mode (either master or slave).
 * @param i2c_sda GPIO number for I2C SDA signal.
 * @param i2c_scl GPIO number for I2C SCL signal.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to init.
 */
esp_err_t i2cbus_init(i2c_port_t i2c_port, i2c_mode_t i2c_mode, gpio_num_t i2c_sda, gpio_num_t i2c_scl);


/**
 * @brief Create a new device on bus.
 * 
 * @param dev pointer to device configurations.
 * @param i2c_port I2C port number lesser than I2C_NUM_MAX.
 * @param addr I2C address to access device on the bus.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to start.
 */
esp_err_t i2cbus_create(i2cbus_t *dev, i2c_port_t i2c_port, uint8_t addr);


/**
 * @brief Delete a device on bus.
 * 
 * @param dev pointer to device configurations.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to delete.
 *     - ESP_ERR_TIMEOUT: bus is busy.
 */
esp_err_t i2cbus_delete(i2cbus_t *dev);


/**
 * @brief Write data to device at specific register.
 * 
 * @param dev pointer to device configurations.
 * @param reg register address to write.
 * @param reg_size sizeof register.
 * @param data data to write on device.
 * @param data_size sizeof data.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to write, device not found.
 *     - ESP_ERR_INVALID_ARG: invalid argument.
 *     - ESP_ERR_TIMEOUT: bus is busy.
 */
esp_err_t i2cbus_write_reg(i2cbus_t *dev, uint8_t *reg, size_t reg_size, uint8_t *data, size_t data_size);


/**
 * @brief Read data from device at specific register.
 * 
 * @param dev pointer to device configurations.
 * @param reg register address  to read.
 * @param reg_size sizeof register.
 * @param data data pointer to send read data.
 * @param data_size sizeof data read.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to write, device not found.
 *     - ESP_ERR_INVALID_ARG: invalid argument.
 *     - ESP_ERR_TIMEOUT: bus is busy.
 */
esp_err_t i2cbus_read_reg(i2cbus_t *dev, uint8_t *reg, size_t reg_size, uint8_t *data, size_t data_size);


/**
 * @brief Write data to device.
 * 
 * @param dev pointer to device configurations.
 * @param data data to read from device.
 * @param data_size sizeof data read.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to write, device not found.
 *     - ESP_ERR_INVALID_ARG: invalid argument.
 *     - ESP_ERR_TIMEOUT: bus is busy.
 */
esp_err_t i2cbus_write(i2cbus_t *dev, uint8_t *data, size_t data_size);


/**
 * @brief Read data from device.
 * 
 * @param dev pointer to device configurations.
 * @param data data pointer to send read data.
 * @param data_size sizeof data read.
 *
 * @return 
 *     - ESP_OK: success.
 *     - ESP_FAIL: fail to write, device not found.
 *     - ESP_ERR_INVALID_ARG: invalid argument.
 *     - ESP_ERR_TIMEOUT: bus is busy.
 */
esp_err_t i2cbus_read(i2cbus_t *dev, uint8_t *data, size_t data_size);

/**@}*/

#ifdef __cplusplus
}
#endif