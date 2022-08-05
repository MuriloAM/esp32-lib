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
 * @file lcd_i2c_const.h
 * @defgroup lcd_i2c_const lcd_i2c_const
 * @{
 *
 * @brief 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DELAY_MS    1000
#define DELAY_PWON  (15 * DELAY_MS)
#define DELAY_RST   (5 * DELAY_MS)
#define DELAY_EN    (1)
#define DELAY_CLR   (2 * DELAY_MS)

// LCD byte pins according PCF8574A connections
#define LCD_BIT_RS  0x0
#define LCD_BIT_RW  0x1
#define LCD_BIT_EN  0x2
#define LCD_BIT_BKL 0x3
#define LCD_BIT_DB4 0x4
#define LCD_BIT_DB5 0x5
#define LCD_BIT_DB6 0x6
#define LCD_BIT_DB7 0x7

// command table values based on LCD1602A datasheet 
// https://cdn-shop.adafruit.com/datasheets/TC1602A-01T.pdf
#define LCD_CLR_DISPLAY 0x01
#define LCD_DISPLAY_OFF 0x08
#define LCD_DISPLAY_ON  0x0C

#define LCD_WRITE_TO_LEFT   0x04
#define LCD_WRITE_TO_RIGHT  0x06

#define LCD_CURSOR_ON   0x0E
#define LCD_CURSOR_OFF  0x0C
#define LCD_CURSOR_MOVE_LEFT    0x10
#define LCD_CURSOR_MOVE_RIGHT   0x14
#define LCD_CURSOR_HOME 0x02
#define LCD_CURSOR_BLINK    0x0D
#define LCD_CURSOR_BLINK2   0x0F

#define LCD_DISPLAY_MOVE_LEFT   0x18
#define LCD_DISPLAY_MOVE_RIGHT  0x1C

#define LCD_CONFIG_8BIT_RST 0x30
#define LCD_CONFIG_4BIT_RST 0x20
#define LCD_CONFIG_8BIT_1LINE_5X7   0x30
#define LCD_CONFIG_8BIT_1LINE_5X10  0x34
#define LCD_CONFIG_8BIT_2LINE_5X7   0x38
#define LCD_CONFIG_8BIT_2LINE_5X10  0x3C
#define LCD_CONFIG_4BIT_1LINE_5X7   0x20
#define LCD_CONFIG_4BIT_1LINE_5X10  0x24
#define LCD_CONFIG_4BIT_2LINE_5X7   0x28
#define LCD_CONFIG_4BIT_2LINE_5X10  0x28

#define LCD_SET_CGRAM_ADDR  0x40
#define LCD_SET_DDRAM_ADDR  0x80

#define LCD_MAX_NUM CONFIG_LCD_MAX_NUM      /*<! maximum display on i2c bus */

// macros
#define MOVE_NIBBLE (4)
#define SET_BIT(y,bit)  (y |=(1<<bit))
#define CLR_BIT(y,bit)  (y &=~(1<<bit))
#define CPL_BIT(y,bit)  (y ^=(1<<bit))
#define TST_BIT(y,bit)  ((y &(1<<bit)) >> bit)
#define HIGH_NIBBLE(y)  (y & 0xF0)
#define LOW_NIBBLE(y)   (y & 0x0F)
#define SHFT_LEFT(y,x)  (y << x)
#define SHFT_RIGHT(y,x) (y >> x)

typedef enum{
    LCD_I2C_INSTRUCTION = 0,    /*!< LCD instruction register access */
    LCD_I2C_DATA = 1            /*!< LCD data register access */
} lcd_i2c_reg_t;

#define LCD_MAX_NUM CONFIG_LCD_MAX_NUM      /*<! maximum display on i2c bus */

/**@}*/

#ifdef __cplusplus
}
#endif