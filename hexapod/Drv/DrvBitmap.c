/*
 * DrvBitmap.c
 *
 * Created: 23/10/2016 12:58:07
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "DrvBitmap.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

void DrvBitmapGetBitmapBattery100( SBitmap *bmp );
void DrvBitmapGetBitmapBattery75( SBitmap *bmp );
void DrvBitmapGetBitmapBattery50( SBitmap *bmp );
void DrvBitmapGetBitmapBattery25( SBitmap *bmp );
void DrvBitmapGetBitmapBattery0( SBitmap *bmp );

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
uint8_t bitmapBattery100[14U] = 
{
	XXXXXXXX,XXXXXXX_,
	X_______,______X_,
	X_XXX_XX,X_XXX_XX,
	X_XX_XXX,_XXX__XX,
	X_X_XXX_,XXX_X_XX,
	X_______,______X_,
	XXXXXXXX,XXXXXXX_,
};

uint8_t bitmapBattery75[14U] = 
{
	XXXXXXXX,XXXXXXX_,
	X_______,______X_,
	X_XXX_XX,X_XXX_XX,
	X_XX_XXX,_XXX__XX,
	X_X_XXX_,XXX___XX,
	X_______,______X_,
	XXXXXXXX,XXXXXXX_,
};

uint8_t bitmapBattery50[14U] = 
{
	XXXXXXXX,XXXXXXX_,
	X_______,______X_,
	X_XXX_XX,X_____XX,
	X_XX_XXX,______XX,
	X_X_XXX_,______XX,
	X_______,______X_,
	XXXXXXXX,XXXXXXX_,
};

uint8_t bitmapBattery25[14U] = 
{
	XXXXXXXX,XXXXXXX_,
	X_______,______X_,
	X_XXX___,______XX,
	X_XX____,______XX,
	X_X_____,______XX,
	X_______,______X_,
	XXXXXXXX,XXXXXXX_,
};

uint8_t bitmapBattery0[14U] = 
{
	XXXXXXXX,XXXXXXX_,
	X_______,______X_,
	X_______,______XX,
	X_______,______XX,
	X_______,______XX,
	X_______,______X_,
	XXXXXXXX,XXXXXXX_,
};


uint8_t bitmapArrowRight[12U] = 
{
	________,___XXX__,
	________,____XXX_,
	XXXXXXXX,XXXXXXXX,
	XXXXXXXX,XXXXXXXX,
	________,____XXX_,
	________,___XXX__,
};

uint8_t bitmapArrowLeft[12U] = 
{
	__XXX___,________,
	_XXX____,________,
	XXXXXXXX,XXXXXXXX,
	XXXXXXXX,XXXXXXXX,
	_XXX____,________,
	__XXX___,________,
};

uint8_t bitmapArrowUp[12U] = 
{
	_______X,X_______,
	______XX,XX______,
	_____XXX,XXX_____,
	_______X,X_______,
	_______X,X_______,
	_______X,X_______,
};

uint8_t bitmapArrowDown[12U] = 
{
	_______X,X_______,
	_______X,X_______,
	_______X,X_______,
	_____XXX,XXX_____,
	______XX,XX______,
	_______X,X_______,
};

uint8_t bitmapArrowCenter[12U] = 
{
	_______X,X_______,
	______XX,XX______,
	_____XXX,XXX_____,
	_____XXX,XXX_____,
	______XX,XX______,
	_______X,X_______,
};

void DrvBitmapGetBitmapArrow( SBitmap *bmp, EBitmapArrow value )
{
	if(value == ARROW_UP)	bmp->bmp = bitmapArrowUp;
	else if(value == ARROW_DOWN)	bmp->bmp = bitmapArrowDown;
	else if(value == ARROW_LEFT)	bmp->bmp = bitmapArrowLeft;
	else if(value == ARROW_RIGHT)	bmp->bmp = bitmapArrowRight;
	else if(value == ARROW_CENTER)	bmp->bmp = bitmapArrowCenter;
	bmp->width = 8U * 2U;
	bmp->height = 6U;
}

void DrvBitmapGetBitmapBatteryValue( SBitmap *bmp, uint8_t value )
{
	if(value == 100)	bmp->bmp = bitmapBattery100;
	else if(value >= 75)	bmp->bmp = bitmapBattery75;
	else if(value >= 50)	bmp->bmp = bitmapBattery50;
	else if(value >= 25)	bmp->bmp = bitmapBattery25;
	else bmp->bmp = bitmapBattery0;
	bmp->width = 8U * 2U;
	bmp->height = 7U;
}