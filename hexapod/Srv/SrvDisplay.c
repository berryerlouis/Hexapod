////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"


#include "Drv/DrvTick.h"

#include "SrvDisplay.h"
#include "SrvBattery.h"
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

//display string on screen
Boolean SrvDisplayScreenShowString ( char * data );
//display line on screen
Boolean SrvDisplayScreenShowProgressBar ( uint8_t percentage ) ;
Boolean SrvDisplayScreenHideProgressBar ( void );
void SrvDisplayScreenShowBattery ( uint8_t percentage );
void SrvDisplayScreenBatteryManagement ( void );
void SrvDisplayScreenShowArrow ( void );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint32_t prevMillisDisplay = 0U;
uint8_t tempPercentage = 0U;
uint8_t batteryLevel = 100U;
Boolean toggleBatteryLogo = FALSE;
uint8_t initStepDisplay = 0U;
EBitmapArrow direction;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////



//Fonction d'initialisation
Boolean SrvDisplayInit ( void ) 
{
	prevMillisDisplay = 0U;
	tempPercentage = 0U;
	batteryLevel = 100U;
	toggleBatteryLogo = FALSE;
	initStepDisplay = 0U;
	direction = ARROW_CENTER;
	return DrvSSD1306Init(SSD1306_ADDRESS);
}

//Fonction de dispatching d'evenements
void SrvDisplayUpdate (void)
{
	if( initStepDisplay == 0U )
	{
		//up to 100
		if(SrvDisplayScreenShowProgressBar(100))
		{
			//go to next step 
			initStepDisplay = 1U;
			//hide progress bar
			SrvDisplayScreenHideProgressBar();
			//show string
			SrvDisplayScreenShowString("Init done !");
		}
		
	}
	else if( initStepDisplay == 1U )
	{
		//next step in 1s
		if ((DrvTickGetTimeMs() - prevMillisDisplay) > 1000U)
		{
			prevMillisDisplay = DrvTickGetTimeMs();
			//clear buffer
			DrvSSD1306ClearBuffer();
				
			//go to next step 
			initStepDisplay = 2U;	
		}
	}
	else if( initStepDisplay == 2U )
	{
		SrvDisplayScreenBatteryManagement();
		SrvDisplayScreenShowArrow();
		
		char tab[5U];
		itoa (batteryLevel, tab, 10);
		SrvDisplayScreenShowString(tab);
	}
}


//display direction on screen
void SrvDisplayScreenShowArrow ( void )
{
	SBitmap bmp;
	DrvBitmapGetBitmapArrow(&bmp,direction);
	DrvSSD1306DrawBitmap(&bmp,SCREEN_WIDTH - bmp.width,0,COLOR_WHITE);
	DrvSSD1306Update();
}

//display battery on screen
void SrvDisplayScreenBatteryManagement ( void )
{
	if ((DrvTickGetTimeMs() - prevMillisDisplay) > 250U)
	{
		prevMillisDisplay = DrvTickGetTimeMs();

		batteryLevel = SrvBatteryGetValue();
		if( batteryLevel == 0U )
		{
			batteryLevel = 100U;
		}
		else if( batteryLevel <= 25U )
		{
			if(toggleBatteryLogo == TRUE)
			{
				toggleBatteryLogo = FALSE;
				SrvDisplayScreenShowBattery(batteryLevel);
			}
			else
			{
				toggleBatteryLogo = TRUE;
				SrvDisplayScreenShowBattery(25);
			}
		}
		else
		{
			//show battery
			SrvDisplayScreenShowBattery(batteryLevel);
		}
		
	}
}

//display battery on screen
void SrvDisplayScreenShowBattery ( uint8_t percentage )
{
	SBitmap bmp;
	DrvBitmapGetBitmapBatteryValue(&bmp,percentage);
	DrvSSD1306DrawBitmap(&bmp,0,0,COLOR_WHITE);
	DrvSSD1306Update();
}

//display string on screen
Boolean SrvDisplayScreenShowString ( char * data )
{
	uint8_t x = 5U;
	uint8_t y = 10U;
	
	DrvSSD1306EraseArea(x,y,CHAR_WIDTH * 10U,CHAR_HEIGHT);
	while ( *data )
	{
		DrvSSD1306DrawChar(*data++, x, y, COLOR_WHITE);
		x += CHAR_WIDTH;
	}
	DrvSSD1306Update();
	return TRUE;
}
//display line on screen
Boolean SrvDisplayScreenHideProgressBar ( void )
{
	#define WAITBAR_HEIGHT	5U
	//rectangle of the contour of progress bar
	DrvSSD1306DrawRectangle(
	0,
	(SCREEN_HEIGHT / 2U) - WAITBAR_HEIGHT + 4U,
	SCREEN_WIDTH- 1U,
	(SCREEN_HEIGHT / 2U) + WAITBAR_HEIGHT,
	COLOR_BLACK );
	//show value
	for( uint8_t loop_x = 0U ; loop_x < SCREEN_WIDTH ; loop_x++ )
	{
		for( uint8_t loop_y = 0U ; loop_y < WAITBAR_HEIGHT ; loop_y++ )
		{
			DrvSSD1306DrawPixel(loop_x, (SCREEN_HEIGHT / 2U) + loop_y, COLOR_BLACK);
		}
	}
	DrvSSD1306Update();
	return TRUE;
}

//display line on screen
Boolean SrvDisplayScreenShowProgressBar ( uint8_t percentage )
{
	#define WAITBAR_HEIGHT	5U
	
	//every 10ms progress bar value increment
	if ((DrvTickGetTimeMs() - prevMillisDisplay) > 10U)
	{
		prevMillisDisplay = DrvTickGetTimeMs();
		//stop at percentage
		if( tempPercentage > ((percentage * SCREEN_WIDTH)/100) )
		{
			return TRUE;
		}
		else
		{
			//rectangle of the contour of progress bar
			DrvSSD1306DrawRectangle(
			0,
			(SCREEN_HEIGHT / 2U) - WAITBAR_HEIGHT + 4U,
			SCREEN_WIDTH- 1U,
			(SCREEN_HEIGHT / 2U) + WAITBAR_HEIGHT,
			COLOR_WHITE );
			//show value
			for( uint8_t loop_y = 0U ; loop_y < WAITBAR_HEIGHT ; loop_y++ )
			{
				DrvSSD1306DrawPixel(tempPercentage, (SCREEN_HEIGHT / 2U) + loop_y, COLOR_WHITE);
			}
			tempPercentage++;
			DrvSSD1306Update();
		}
	}
	return FALSE;
}