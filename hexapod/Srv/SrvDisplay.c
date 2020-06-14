////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"


#include "Drv/DrvTick.h"
#include "Cmps/CmpSSD1306.h"

#include "SrvDisplay.h"
#include "SrvBattery.h"
#include "SrvDetection.h"
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

Boolean SrvDisplayScreenShowString (uint8_t x, uint8_t y, char *data );
Boolean SrvDisplayScreenShowProgressBar ( uint8_t percentage ) ;
Boolean SrvDisplayScreenHideProgressBar ( void );
void SrvDisplayScreenShowBattery ( uint8_t percentage );
void SrvDisplayScreenBatteryManagement ( void );
void SrvDisplayScreenShowArrow ( void );
void SrvDisplayScreenShowCommunication ( void );
void SrvDisplayScreenShowUltrason ( void );

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
uint32_t prevMillisUpdateDisplay = 0U;
uint32_t prevMillisDisplay = 0U;
uint8_t tempPercentage = 0U;
SBatteryInfo *batteryInfo;
Boolean toggleBatteryLogo = FALSE;
uint8_t initStepDisplay = 0U;
EBitmapArrow direction;
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////



//Fonction d'initialisation
Boolean SrvDisplayInit ( void ) 
{
	prevMillisDisplay = 0U;
	tempPercentage = 0U;
	toggleBatteryLogo = FALSE;
	initStepDisplay = 0U;
	direction = ARROW_CENTER;
	return CmpSSD1306Init(SSD1306_ADDRESS);
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
			SrvDisplayScreenShowString(5U,10U,"Init done !");
		}
		
	}
	else if( initStepDisplay == 1U )
	{
		//next step in 1s
		if ((DrvTickGetTimeMs() - prevMillisDisplay) > 1000U)
		{
			prevMillisDisplay = DrvTickGetTimeMs();
			//clear buffer
			CmpSSD1306ClearBuffer();
				
			CmpSSD1306DrawLine(0U,9U,SCREEN_WIDTH,9U,COLOR_WHITE);
			SrvDisplayScreenShowCommunication();
			//go to next step 
			initStepDisplay = 2U;	
		}
	}
	else if( initStepDisplay == 2U )
	{
		SrvDisplayScreenBatteryManagement();
		//go to next step
		initStepDisplay = 3U;
	}
	else if( initStepDisplay == 3U )
	{
		SrvDisplayScreenShowUltrason();
		//go to next step
		initStepDisplay = 4U;
	}
	else if( initStepDisplay == 4U )
	{
		SrvDisplayScreenShowArrow();
		//go to next step
		initStepDisplay = 2U;
	}
	
	
	//update every 20ms
	if ((DrvTickGetTimeMs() - prevMillisUpdateDisplay) > 500U)
	{
		prevMillisUpdateDisplay = DrvTickGetTimeMs();
		CmpSSD1306Update();
	}
}

void SrvDisplaySetDirection (EBitmapArrow dir) 
{
	direction = dir;
}

void SrvDisplayScreenShowUltrason ( void )
{
	char tabUs0[5U] = {0U};
	char tabUs1[5U] = {0U};
	itoa (SrvDetectionGetDistanceUS(E_ULTRASON_0), tabUs0, 10);
	size_t len = strlen(tabUs0);
	tabUs0[len] = 'c';
	tabUs0[len + 1U] = 'm';
	SrvDisplayScreenShowString(5U,11U,tabUs0);
	itoa (SrvDetectionGetDistanceUS(E_ULTRASON_1), tabUs1, 10);
	len = strlen(tabUs1);
	tabUs1[len] = 'c';
	tabUs1[len + 1U] = 'm';
	SrvDisplayScreenShowString(SCREEN_WIDTH - 26U ,11U,tabUs1);
}

void SrvDisplayScreenShowCommunication ( void )
{
	SBitmap bmp;
	DrvBitmapGetBitmapCommunication(&bmp);
	CmpSSD1306DrawBitmap(&bmp,SCREEN_WIDTH - bmp.width - 20U, 0U, COLOR_WHITE);
}

//display direction on screen
void SrvDisplayScreenShowArrow ( void )
{
	SBitmap bmp;
	DrvBitmapGetBitmapArrow(&bmp,direction);
	CmpSSD1306DrawBitmap(&bmp,SCREEN_WIDTH - bmp.width,0,COLOR_WHITE);
}

//display battery on screen
void SrvDisplayScreenBatteryManagement ( void )
{
	if ((DrvTickGetTimeMs() - prevMillisDisplay) > 250U)
	{
		prevMillisDisplay = DrvTickGetTimeMs();

		batteryInfo = SrvBatteryGetStruct();
		if( batteryInfo->percentage <= 25U )
		{
			if(toggleBatteryLogo == TRUE)
			{
				toggleBatteryLogo = FALSE;
				SrvDisplayScreenShowBattery(batteryInfo->percentage);
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
			SrvDisplayScreenShowBattery(batteryInfo->percentage);
		}
		
		char tab[5U] = {0U};
		itoa (batteryInfo->percentage, tab, 10);
		tab[strlen(tab)] = '%';
		SrvDisplayScreenShowString(18U,0U,tab);
	}
}

//display battery on screen
void SrvDisplayScreenShowBattery ( uint8_t percentage )
{
	SBitmap bmp;
	DrvBitmapGetBitmapBatteryValue(&bmp,percentage);
	CmpSSD1306DrawBitmap(&bmp,0,0,COLOR_WHITE);
}

//display string on screen
Boolean SrvDisplayScreenShowString (uint8_t x,uint8_t y, char * data )
{
	
	CmpSSD1306EraseArea(x,y,CHAR_WIDTH * 10U,CHAR_HEIGHT);
	while ( *data )
	{
		CmpSSD1306DrawChar(*data++, x, y, COLOR_WHITE);
		x += CHAR_WIDTH;
	}
	return TRUE;
}
//display line on screen
Boolean SrvDisplayScreenHideProgressBar ( void )
{
	#define WAITBAR_HEIGHT	5U
	//rectangle of the contour of progress bar
	CmpSSD1306DrawRectangle(
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
			CmpSSD1306DrawPixel(loop_x, (SCREEN_HEIGHT / 2U) + loop_y, COLOR_BLACK);
		}
	}
	return TRUE;
}

//display line on screen
Boolean SrvDisplayScreenShowProgressBar ( uint8_t percentage )
{
	#define WAITBAR_HEIGHT	5U
	
	//every 10ms progress bar value increment
	if ((DrvTickGetTimeMs() - prevMillisDisplay) > 30U)
	{
		prevMillisDisplay = DrvTickGetTimeMs();
		//stop at percentage
		if( tempPercentage > ((percentage * SCREEN_WIDTH)/100) )
		{
			tempPercentage = 0U;
			return TRUE;
		}
		else
		{
			//rectangle of the contour of progress bar
			CmpSSD1306DrawRectangle(
			0,
			(SCREEN_HEIGHT / 2U) - WAITBAR_HEIGHT + 4U,
			SCREEN_WIDTH - 1U,
			(SCREEN_HEIGHT / 2U) + WAITBAR_HEIGHT,
			COLOR_WHITE );
			//show value
			for( uint8_t loop_y = 0U ; loop_y < WAITBAR_HEIGHT ; loop_y++ )
			{
				CmpSSD1306DrawPixel(tempPercentage, (SCREEN_HEIGHT / 2U) + loop_y, COLOR_WHITE);
			}
			tempPercentage += 1U;
		}
	}
	return FALSE;
}