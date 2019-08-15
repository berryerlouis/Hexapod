/* 
* DrvSSD1306.h
*
* Created: 17/02/2019 22:53:39
* Author: louis
*/


#ifndef __DRVSSD1306_H__
#define __DRVSSD1306_H__


#include "Conf/ConfHard.h"
#include "Drv/DrvTwi.h"
#include "Drv/DrvFont.h"
#include "Drv/DrvBitmap.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////

#define SSD1306_ADDRESS				0x3CU ///< I2C address of the ssd1306 module

#define COLOR_BLACK                          0U ///< Draw 'off' pixels
#define COLOR_WHITE                          1U ///< Draw 'on' pixels
#define INVERSE                        2U ///< Invert pixels

#define SSD1306_MEMORYMODE          0x20U ///< See datasheet
#define SSD1306_COLUMNADDR          0x21U ///< See datasheet
#define SSD1306_PAGEADDR            0x22U ///< See datasheet
#define SSD1306_SETCONTRAST         0x81U ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8DU ///< See datasheet
#define SSD1306_SEGREMAP            0xA0U ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4U ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5U ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6U ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7U ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8U ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAEU ///< See datasheet
#define SSD1306_DISPLAYON           0xAFU ///< See datasheet
#define SSD1306_COMSCANINC          0xC0U ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8U ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3U ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5U ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9U ///< See datasheet
#define SSD1306_SETCOMPINS          0xDAU ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDBU ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00U ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10U ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40U ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01U ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02U ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26U ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27U ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29U ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2AU ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2EU ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2FU ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3U ///< Set scroll range


#define SCREEN_WIDTH	128U // OLED display width, in pixels
#define SCREEN_HEIGHT	32U // OLED display height, in pixels


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

Boolean DrvSSD1306Init(Int8U addr);
void DrvSSD1306DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void DrvSSD1306DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void DrvSSD1306DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void DrvSSD1306DrawCircle(uint16_t xc, uint16_t yc, uint16_t r, uint16_t color);
void DrvSSD1306DrawChar(char c, uint16_t x, uint16_t y, uint8_t color);
void DrvSSD1306DrawString(const char* str, uint16_t x, uint16_t y, uint8_t color);
void DrvSSD1306DrawBitmap(const SBitmap* bmp, uint16_t x, uint16_t y, uint8_t color);
Boolean DrvSSD1306Update( void );
Boolean DrvSSD1306ClearBuffer( void );
Boolean DrvSSD1306ClearDisplay( void );
Boolean DrvSSD1306EraseArea ( uint16_t x, uint16_t y, uint16_t width, uint16_t height );

#endif //__DRVSSD1306_H__