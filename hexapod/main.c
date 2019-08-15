/*
 * main.c
 *
 * Created: 03/07/2012 10:06:16
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"
#include "Drv/DrvTimer.h"
#include "Drv/DrvInterrupt.h"
#include "Drv/DrvLed.h"
#include "Drv/DrvUart.h"
#include "Drv/DrvTwi.h"

#include "Srv/SrvIhm.h"
#include "Srv/SrvWalk.h"
#include "Srv/SrvComm.h"
#include "Srv/SrvBody.h"
#include "Srv/SrvDisplay.h"
#include "Srv/SrvBattery.h"

////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES////////////////////////////////////////
//permits to compute the main loop time
Int32U lastLoopTime = 0U;

//fonction principale
int main(void)
{	
	// ********************* Reset Cause **********************************************
	//imu.status.lastResetCause = MCUSR;
	//reset status register
	MCUSR = 0U;
	
	// ********************* Interrupt Vectors ****************************************
	// Enable change of interrupt vectors
	MCUCR |= (1<<IVCE);
	// Move interrupts to app flash section
	MCUCR &= ~(1<<IVSEL);

	// ********************* Interrupt Disable ****************************************
	DrvInterruptClearAllInterrupts();
	
	// ********************* WatchDog Enable ******************************************
	wdt_enable(WDTO_1S);
	
	// ********************* Interrupt Enable *****************************************
	DrvInterruptSetAllInterrupts();
	
	// ********************* Drivers core init ****************************************
	DrvTickInit();
	DrvTimerInit();
	DrvEepromInit();
	DrvUartInit( E_UART_0, UART_SPEED_115200 );
	DrvTwiInit(TWI_SPEED_400K);
	// ********************* Services core init ***************************************
	SrvBatteryInit();
	SrvIhmInit();
	SrvDisplayInit();
	SrvCommInit();
	SrvBodyInit();
	
	// ********************* Start IHM ************************************************
	SrvIhmPlatformInitStart() ;
		
	// ********************* Reset time ***********************************************
	DrvTimerTickReset();
	lastLoopTime = DrvTickGetTimeUs();
		
	// ********************* Led ok blink mode ****************************************
	SrvIhmPlatformInitDone();
	
	//SrvWalkInit();
		
	// ********************* Eeprom is now configured for next startup ****************
	DrvEepromSetConfiguration();
	
	while(TRUE)
	{
		// ********************* Watchdog *********************************************
		wdt_enable(WDTO_1S);
				
		// ********************* Calcul du temps de cycle *****************************

		Int32U now = DrvTickGetTimeUs();
		//imu.status.loopTime = now - lastLoopTime;
		lastLoopTime = now - lastLoopTime;
		//imu.status.lifeTime = now / 1000000U;
				

		// ********************* Call services******************
		SrvBatteryUpdate();
		SrvIhmDispatcher();
		SrvCommUpdate();
		SrvBodyUpdate();
		SrvDisplayUpdate();
	}
}
























/*FUSES = 
    {
        .low = LFUSE_DEFAULT,
        .high = (FUSE_SPIEN & FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE  & FUSE_JTAGEN)
        .extended = EFUSE_DEFAULT,
    };*/

