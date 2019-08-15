/* 
* DrvSSD1306.c
*
* Created: 17/02/2019 22:53:39
* Author: louis
*/


#include "DrvSSD1306.h"
#include "DrvFont.h"
#include "DrvBitmap.h"

#define BUFFER_DISPLAY_LENGTH	SCREEN_WIDTH * ((SCREEN_HEIGHT + 7U) / 8U)

Int8U addr_i2c = 0xFFU;
Int8U bufferScreen[BUFFER_DISPLAY_LENGTH];
Int8U vccstate =  SSD1306_SWITCHCAPVCC;

void  DrvSSD1306SendDatum(Int8U c) ;
void  DrvSSD1306SendData( Int8U *c, Int8U n);


Boolean DrvSSD1306Init(Int8U addr)
{
	addr_i2c = addr;
	memset(bufferScreen, 0,BUFFER_DISPLAY_LENGTH);
	
	vccstate = SSD1306_SWITCHCAPVCC;
	static Int8U init1[] = 
	{
		SSD1306_DISPLAYOFF,                   // 0xAE
		SSD1306_SETDISPLAYCLOCKDIV,           // 0xD5
		0x80,                                 // the suggested ratio 0x80
		SSD1306_SETMULTIPLEX 
	};               // 0xA8
	DrvSSD1306SendData(init1, sizeof(init1));
	DrvSSD1306SendDatum(SCREEN_HEIGHT - 1);
	
	static Int8U init2[] =
		{
		SSD1306_SETDISPLAYOFFSET,             // 0xD3
		0x0,                                  // no offset
		SSD1306_SETSTARTLINE | 0x0,           // line #0
		SSD1306_CHARGEPUMP 
	};                 // 0x8D
	DrvSSD1306SendData(init2, sizeof(init2));

	DrvSSD1306SendDatum((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

	static Int8U init3[] = 
	{
		SSD1306_MEMORYMODE,                   // 0x20
		0x00,                                 // 0x0 act like ks0108
		SSD1306_SEGREMAP | 0x1,
		SSD1306_COMSCANDEC 
	};
	DrvSSD1306SendData(init3, sizeof(init3));

	static Int8U init4a[] = 
	{
		SSD1306_SETCOMPINS,                 // 0xDA
		0x02,
		SSD1306_SETCONTRAST,                // 0x81
		0x8F 
	};
	DrvSSD1306SendData(init4a, sizeof(init4a));
		

	DrvSSD1306SendDatum(SSD1306_SETPRECHARGE); // 0xd9
	DrvSSD1306SendDatum((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
	static Int8U init5[] = {
		SSD1306_SETVCOMDETECT,               // 0xDB
		0x40,
		SSD1306_DISPLAYALLON_RESUME,         // 0xA4
		SSD1306_NORMALDISPLAY,               // 0xA6
		SSD1306_DEACTIVATE_SCROLL,
		SSD1306_DISPLAYON };                 // Main screen turn on
	DrvSSD1306SendData(init5, sizeof(init5));
		
	DrvSSD1306ClearDisplay();
	DrvSSD1306Update();
	return TRUE;
}
Boolean DrvSSD1306ClearDisplay()
{
	static Int8U dlist1[] = {
		SSD1306_PAGEADDR,
		0,                         // Page start address
		0xFF,                      // Page end (not really, but works here)
		SSD1306_COLUMNADDR,
		0 
	};                       // Column start address
	DrvSSD1306SendData(dlist1, sizeof(dlist1));
	DrvSSD1306SendDatum(SCREEN_WIDTH - 1); // Column end address


	Int16U count = BUFFER_DISPLAY_LENGTH;
	
	while(count >= 32)
	{
		DrvTwiFillRegBuf(addr_i2c,0x40U,0x00U,32U);
		count -= 32U;
	}
	if(count > 0 )
	{
		DrvTwiFillRegBuf(addr_i2c,0x40U,0x00U,BUFFER_DISPLAY_LENGTH - count);
	}
	
	return TRUE;
}

Boolean DrvSSD1306ClearBuffer( void )
{
	memset(bufferScreen, 0,BUFFER_DISPLAY_LENGTH);
	return TRUE;
}
	
Boolean DrvSSD1306Update( void )
{
	static Int8U dlist2[] = {
		SSD1306_PAGEADDR,
		0,                         // Page start address
		0xFF,                      // Page end (not really, but works here)
		SSD1306_COLUMNADDR,
		0
	};
	// Column start address
	DrvSSD1306SendData(dlist2, sizeof(dlist2));
	DrvSSD1306SendDatum(SCREEN_WIDTH - 1); // Column end address

	Int16U count = BUFFER_DISPLAY_LENGTH;
	
	while(count >= 32)
	{
		DrvTwiWriteRegBuf(addr_i2c,0x40U,&bufferScreen[ BUFFER_DISPLAY_LENGTH - count ],32U);
		count -= 32U;
	}
	if(count > 0 )
	{
		DrvTwiWriteRegBuf(addr_i2c,0x40U,&bufferScreen[ BUFFER_DISPLAY_LENGTH - count ],BUFFER_DISPLAY_LENGTH - count);
	}
	return TRUE;
}

//display line on screen
Boolean DrvSSD1306EraseArea ( uint16_t x, uint16_t y, uint16_t width, uint16_t height )
{
	//show value
	for( uint8_t loop_x = 0U ; loop_x < width ; loop_x++ )
	{
		for( uint8_t loop_y = 0U ; loop_y < height ; loop_y++ )
		{
			DrvSSD1306DrawPixel(x + loop_x, y + loop_y, COLOR_BLACK);
		}
	}
	return TRUE;
}

void DrvSSD1306DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if((x < SCREEN_WIDTH) && (y < SCREEN_HEIGHT))
	{
		switch(color) 
		{
			case COLOR_WHITE:   bufferScreen[x + (y/8)*SCREEN_WIDTH] |=  (1 << (y&7)); break;
			case COLOR_BLACK:   bufferScreen[x + (y/8)*SCREEN_WIDTH] &= ~(1 << (y&7)); break;
			case INVERSE: bufferScreen[x + (y/8)*SCREEN_WIDTH] ^=  (1 << (y&7)); break;
		}
	}
}
void DrvSSD1306DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	if(
		(x1 <= SCREEN_WIDTH) && (x2 <= SCREEN_WIDTH) &&
		(y1 <= SCREEN_HEIGHT) && (y2 <= SCREEN_HEIGHT) 
	)
	{
		uint16_t dx = x2 - x1;
		uint16_t dy = y2 - y1;
		if( dx != 0U )
		{
			for( uint16_t x = 0U ; x < x2 ; x++ )
			{
				uint16_t y = y1 + dy * (x - x1) / dx;
				DrvSSD1306DrawPixel(x, y, color);
			}
		}
		else
		{
			for( uint16_t y = y1 ; y < y2 ; y++ )
			{
				DrvSSD1306DrawPixel(x1, y, color);
			}
		}
	}
}
void DrvSSD1306DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	if(
		(x1 < SCREEN_WIDTH) && (x2 < SCREEN_WIDTH) &&
		(y1 < SCREEN_HEIGHT) && (y2 < SCREEN_HEIGHT)
	)
	{
		DrvSSD1306DrawLine(x1, y1, x2, y1, color);
		DrvSSD1306DrawLine(x1, y2, x2, y2, color);
		DrvSSD1306DrawLine(x1, y1, x1, y2, color);
		DrvSSD1306DrawLine(x2, y1, x2, y2, color);
	}
}

void DrvSSD1306DrawCircle(uint16_t xc,uint16_t yc,uint16_t r, uint16_t color)
{
	uint16_t x,y,p;
	x = 0;
	y = r;
	DrvSSD1306DrawPixel(xc+x,yc-y,color);

	p = 3 - ( 2 * r );

	for( x = 0 ; x <= y ; x++ )
	{
		if ( p < 0 )
		{
			p = ( p + ( 4 * x ) + 6);
		}
		else
		{
			y -= 1;
			p += ( ( 4 * ( x - y ) + 10 ) );
		}

		DrvSSD1306DrawPixel(xc+x, yc-y, color);
		DrvSSD1306DrawPixel(xc-x, yc-y, color);
		DrvSSD1306DrawPixel(xc+x, yc+y, color);
		DrvSSD1306DrawPixel(xc-x, yc+y, color);
		DrvSSD1306DrawPixel(xc+y, yc-x, color);
		DrvSSD1306DrawPixel(xc-y, yc-x, color);
		DrvSSD1306DrawPixel(xc+y, yc+x, color);
		DrvSSD1306DrawPixel(xc-y, yc+x, color);
	}
}

void DrvSSD1306DrawChar(char c, uint16_t x, uint16_t y, uint8_t color)
{
	uint8_t i,j;

	// Convert the character to an index
	c = c & 0x7FU;
	if (c < ' ') 
	{
		c = 0U;
	}
	else 
	{
		c -= ' ';
	}

	// 'font' is a multidimensional array of [96][char_width]
	// which is really just a 1D array of size 96*char_width.
	const uint8_t* chr = font[c];

	// Draw pixels
	for (j = 0; j < CHAR_WIDTH; j++) 
	{
		for (i = 0; i < CHAR_HEIGHT; i++)
		{
			if (chr[j] & (1<<i)) 
			{
				DrvSSD1306DrawPixel(x+j, y+i, color);
			}
			else
			{
				DrvSSD1306DrawPixel(x+j, y+i, color==COLOR_BLACK ? COLOR_WHITE : COLOR_BLACK);
			}
		}
	}
}

void DrvSSD1306DrawString(const char* str, uint16_t x, uint16_t y, uint8_t color) 
{
	while ( *str ) 
	{
		DrvSSD1306DrawChar(*str++, x, y, color);
		x += CHAR_WIDTH + 1U;
	}
}

void DrvSSD1306DrawBitmap(const SBitmap* bmp, uint16_t x, uint16_t y, uint8_t color)
{
	// Draw pixels
	for (uint16_t i = 0; i < bmp->height; i++)
	{
		for (uint16_t j = 0; j < bmp->width; j++)
		{
			if (bmp->bmp[(j/8U)+(i*(bmp->width/8U))] & (0x80>>(j%8U)))
			{
				DrvSSD1306DrawPixel(x+j, y+i, color);
			}
			else
			{
				DrvSSD1306DrawPixel(x+j, y+i, color==COLOR_BLACK ? COLOR_WHITE : COLOR_BLACK);
			}
		}
	}
}

void DrvSSD1306SendData( Int8U *c, Int8U n)
{
	DrvTwiWriteRegBuf(addr_i2c,0x00U,c,n);

}
void DrvSSD1306SendDatum(Int8U c) 
{
	DrvTwiWriteReg(addr_i2c,0x00U,c);
}