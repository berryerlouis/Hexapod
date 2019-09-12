/*
 * DrvBitap.h
 *
 * Created: 23/10/2016 12:58:24
 *  Author: berryer
 */ 


#ifndef DRVBITMAP_H_
#define DRVBITMAP_H_

#include "Tools/tools_typedefs.h"

////////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define CHAR_WIDTH	5U
#define CHAR_HEIGHT 8U

///////////////////////////////////////////////PUBLIC ENUM/////////////////////////////////////////
typedef enum EEBitmapArrow
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
	ARROW_CENTER
}EBitmapArrow;
/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct SSBitmap  
{
	uint8_t * bmp;
	uint16_t width;
	uint16_t height;
}SBitmap;
/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
void DrvBitmapGetBitmapBattery( SBitmap *bmp );
void DrvBitmapGetBitmapBatteryValue( SBitmap *bmp, uint8_t value );
void DrvBitmapGetBitmapArrow( SBitmap *bmp, EBitmapArrow value );
void DrvBitmapGetBitmapCommunication( SBitmap *bmp );
#endif /* DRVBITMAP_H_ */